#include "http/CGIHandler.hpp"

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include "http/Request.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

#include <cstdlib>
#include <cstring>

extern char** environ;

namespace http {

	const time_t CGIHandler::TIMEOUT = 30;

	CGIHandler::CGIHandler()
		: ARequestHandler()
		, m_processState(READING)
		, m_pid(-1)
		, m_inputPipe()
		, m_outputPipe()
		, m_responseParser()
		, m_bytesWritten(0)
		, m_startTime(time(NULL)) {}

	CGIHandler::~CGIHandler() {}

	void CGIHandler::handle(const Request* request) {
		switch (m_state) {
			case PREPROCESS:
				preporcess(request);
				m_state = PROCESS;
			case PROCESS:
				if (!process(request)) {
					m_state = DONE;
				}
				break;
			case DONE:
				break;
		}
	}

	void CGIHandler::preporcess(const Request* request) {
		m_startTime = time(NULL);
		prepareEnviorment(request);
		launchScript(request);

		if (request->getMethod() == POST) {
			m_processState = WRITING;
		} else {
			m_processState = READING;
			m_inputPipe.close();
		}
	}

	bool CGIHandler::process(const Request* request) {
		time_t now = time(NULL);
		if (now - m_startTime > TIMEOUT) {
			kill(m_pid, SIGKILL);
			m_pid = -1;
			throw HttpException(GATEWAY_TIMEOUT, "CGI script execution timed out");
		}

		switch (m_processState) {
			case WRITING:
				if (!processWriting(request)) {
					m_processState = READING;
				}
				break;

			case READING:
				if (!processReading()) {
					m_processState = WAITING;
				}
				break;

			case WAITING:
				if (!processWaiting()) {
					return false;
				}
				break;
		}
		return true;
	}

	bool CGIHandler::processWriting(const Request* request) {
		ssize_t bytesWritten = m_inputPipe.write(
			request->getBody().c_str() + m_bytesWritten,
			request->getBody().size() - m_bytesWritten);

		if (bytesWritten == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) { // not allowed
				return true;
			}
			throw HttpException(INTERNAL_SERVER_ERROR, "failed to write to CGI script: " + std::string(std::strerror(errno)));
		}

		m_bytesWritten += bytesWritten;
		if (m_bytesWritten >= request->getBody().size()) {
			std::cerr << "wrote all this shit  " << request->getBody().size() << std::endl;
			m_inputPipe.close();
			return false;
		}

		return true;
	}

	bool CGIHandler::processReading() {
		shared::container::Buffer<ResponseParser::BUFFER_SIZE>& buffer = m_responseParser.getReadBuffer();

		std::size_t available = buffer.prepareWrite();
		ssize_t bytesRead = m_outputPipe.read(buffer.writePtr(), available); // todo: check for errno?! (EWOULDBLOCK? - idk if this is allowed)
		if (bytesRead == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) { // todo:  not allowed
				return true;
			}
			throw HttpException(INTERNAL_SERVER_ERROR, "failed to read from CGI script: " + std::string(std::strerror(errno)));
		} else if (bytesRead == 0 && m_responseParser.isComplete() == false) {
			throw HttpException(BAD_GATEWAY, "incomplete cgi response");
		}
		buffer.advanceWriter(bytesRead);

		if (!m_responseParser.parse()) {
			m_response = m_responseParser.releaseResponse();
			return false;
		}

		return true;
	}

	bool CGIHandler::processWaiting() {
		int status;
		pid_t result = waitpid(m_pid, &status, WNOHANG);
		if (result == -1) {
			throw HttpException(INTERNAL_SERVER_ERROR, "waitpid() failed " + std::string(std::strerror(errno)));
		} else if (result == 0) {
			return true;
		}

		m_pid = -1;
		return false;
	}

	void CGIHandler::reset() {
		m_inputPipe.close();
		m_outputPipe.close();

		if (m_pid != -1) {
			kill(m_pid, SIGKILL);
			m_pid = -1;
		}
	}

	// todo: maybe this is missing some stuff lol
	void CGIHandler::prepareEnviorment(const Request* request) {
		setenv("PATH_INFO", request->getUri().getCgiPathInfo().c_str());
		setenv("SERVER_PROTOCOL", request->getVersion().c_str());
		setenv("QUERY_STRING", request->getUri().getQuery().c_str());
		setenv("REQUEST_METHOD", methodToString(request->getMethod()));
		setenv("SCRIPT_NAME", request->getUri().getPath().substr(9 /* /cgi-bin/ */).c_str());

		if (request->hasHeader("content-length")) {
			setenv("CONTENT_LENGTH", request->getHeader("content-length").front().c_str());
		}
		if (request->hasHeader("content-type")) {
			setenv("CONTENT_TYPE", request->getHeader("content-type").front().c_str());
		}
	}

	void CGIHandler::launchScript(const http::Request* request) {
		m_inputPipe.open();
		m_outputPipe.open();

		m_inputPipe.setNonBlocking(true); // todo this is forbidden
		m_outputPipe.setNonBlocking(true);

		m_pid = fork();
		if (m_pid == -1) {
			throw HttpException(INTERNAL_SERVER_ERROR, "fork() failed " + std::string(std::strerror(errno)));
		}

		if (m_pid == 0) {
			m_inputPipe.closeWriteEnd();
			m_outputPipe.closeReadEnd();

			m_inputPipe.dupReadFd(STDIN_FILENO);
			m_outputPipe.dupWriteFd(STDOUT_FILENO);

			const std::string& scriptPath = request->getUri().getPath().substr(1); // todo: somehow get the corret path
			const std::string& interpreter = getInterpreter(scriptPath);
			char* const argv[] = {
				const_cast<char*>(interpreter.c_str()),
				const_cast<char*>(scriptPath.c_str()),
				NULL};

			LOG_DEBUG("interpreter: " + interpreter + " script path: " + scriptPath);
			execve(interpreter.c_str(), argv, environ);
			LOG_ERROR("execve() failed: " + std::string(std::strerror(errno)));
			// todo: cry
		} else {
			m_inputPipe.closeReadEnd();
			m_outputPipe.closeWriteEnd();
		}
	}

	// very nice hardcoded stuff
	const std::string& CGIHandler::getInterpreter(const std::string& scriptPath) {
		size_t dotPos = scriptPath.find_last_of('.');
		std::string extension = (dotPos != std::string::npos) ? scriptPath.substr(dotPos) : "";

		static std::map<std::string, std::string> interpreters;
		if (interpreters.empty()) {
			interpreters[".py"] = "/usr/bin/python3"; // todo: read this from the env ig
		}

		std::map<std::string, std::string>::const_iterator it = interpreters.find(extension);
		if (it != interpreters.end()) {
			return it->second;
		}

		throw HttpException(NOT_IMPLEMENTED, "unsupported script type: " + extension);
	}

	void CGIHandler::setenv(const char* name, const char* value) const {
		if (::setenv(name, value, true)) {
			throw HttpException(INTERNAL_SERVER_ERROR, "setenv() failed" + std::string(std::strerror(errno)));
		}
	}

} /* namespace http */
