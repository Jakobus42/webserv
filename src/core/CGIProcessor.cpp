#include "core/CGIProcessor.hpp"

#include <signal.h>
#include <sys/wait.h>

#include "core/CGIEventHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

#include <cstdlib>
#include <cstring>
#include <ctime>

// todo: add CGI timeout and interpreters paths to config

namespace core {

	const time_t CGIProcessor::DEFAULT_TIMEOUT = 5; // todo: change this to 30

	CGIProcessor::CGIProcessor(io::Dispatcher& dispatcher)
		: m_dispatcher(dispatcher)
		, m_response(NULL)
		, m_pid(-1)
		, m_inputPipe()
		, m_outputPipe()
		, m_startTime(-1)
		, m_timeout(DEFAULT_TIMEOUT) // todo: switch with config
		, m_ioState(IO_NONE)
		, m_state(EXECUTE) {
		m_inputPipe.open();
		m_outputPipe.open();
	}

	CGIProcessor::~CGIProcessor() { cleanup(); }

	bool CGIProcessor::process(const http::Request& request) {
		if (request.getMethod() != http::GET && request.getMethod() != http::POST) {
			throw http::HttpException(http::BAD_REQUEST, "invalid method for CGI: only POST or GET is allowed");
		}

		switch (m_state) {
			case EXECUTE: {
				executeCGIScript(request);
				m_state = WAIT;
				break;
			}
			case WAIT: {
				if (!waitCGIScript()) {
					m_state = DONE;
				}
				break;
			}
			case DONE: break;
		}
		return !(m_state == DONE);
	}

	http::Response* CGIProcessor::releaseResponse() {
		http::Response* released = m_response;
		m_response = NULL;
		return released;
	}

	void CGIProcessor::reset() {
		cleanup();

		m_inputPipe.open();
		m_outputPipe.open();
		m_state = EXECUTE;
	}

	void CGIProcessor::notifyIOReadCompletion() {
		m_ioState |= IO_READ_COMPLETE;
	}

	void CGIProcessor::notifyIOWriteCompletion() {
		m_ioState |= IO_WRITE_COMPLETE;
	}

	void CGIProcessor::notifyIOError() {
		m_ioState |= IO_ERROR;
	}

	// todo: exit child properly
	// todo: check if script exists before exec - this should maybe happen in router or something
	// todo: somehow get the corret path
	void CGIProcessor::executeCGIScript(const http::Request& request) {
		m_startTime = std::time(NULL);

		m_pid = fork();
		if (m_pid == -1) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "fork() failed " + std::string(std::strerror(errno)));
		}

		if (m_pid == 0) {
			prepareEnviorment(request);

			m_inputPipe.closeWriteEnd();
			m_outputPipe.closeReadEnd();

			m_inputPipe.dupReadFd(STDIN_FILENO);
			m_outputPipe.dupWriteFd(STDOUT_FILENO);

			const std::string& scriptPath = request.getUri().getPath().substr(1);
			const std::string& interpreter = getInterpreter(scriptPath);
			char* const argv[] = {
				const_cast<char*>(interpreter.c_str()),
				const_cast<char*>(scriptPath.c_str()),
				NULL};

			execve(interpreter.c_str(), argv, environ);
			LOG_ERROR("execve() failed: " + std::string(std::strerror(errno)));
		} else {
			m_inputPipe.closeReadEnd();
			m_outputPipe.closeWriteEnd();

			m_dispatcher.registerHandler(m_outputPipe.getReadFd(),
										 new CGIEventHandler(*this, request, m_response),
										 io::AMultiplexer::EVENT_READ | io::AMultiplexer::EVENT_ERROR);
			if (request.getMethod() == http::POST) {
				m_dispatcher.registerHandler(m_inputPipe.getWriteFd(),
											 new CGIEventHandler(*this, request, m_response),
											 io::AMultiplexer::EVENT_WRITE | io::AMultiplexer::EVENT_ERROR);
			} else {
				notifyIOWriteCompletion();
			}
		}
	}

	// todo: get real path from env or config?
	const std::string& CGIProcessor::getInterpreter(const std::string& scriptPath) {
		size_t dotPos = scriptPath.find_last_of('.');
		std::string extension = (dotPos != std::string::npos) ? scriptPath.substr(dotPos) : "";

		static std::map<std::string, std::string> interpreters;
		if (interpreters.empty()) {
			interpreters[".py"] = "/usr/bin/python3";
		}

		std::map<std::string, std::string>::const_iterator it = interpreters.find(extension);
		if (it == interpreters.end()) {
			throw http::HttpException(http::NOT_IMPLEMENTED, "unsupported script type: " + extension);
		}

		return it->second;
	}

	// todo: maybe add some more stuff
	void CGIProcessor::prepareEnviorment(const http::Request& request) {
		setenv("PATH_INFO", request.getUri().getCgiPathInfo().c_str());
		setenv("SERVER_PROTOCOL", request.getVersion().c_str());
		setenv("QUERY_STRING", request.getUri().getQuery().c_str());
		setenv("REQUEST_METHOD", methodToString(request.getMethod()));
		setenv("SCRIPT_NAME", request.getUri().getPath().substr(9 /* /cgi-bin/ */).c_str());

		if (request.hasHeader("content-length")) {
			setenv("CONTENT_LENGTH", request.getHeader("content-length").front().c_str());
		}
		if (request.hasHeader("content-type")) {
			setenv("CONTENT_TYPE", request.getHeader("content-type").front().c_str());
		}
	}

	bool CGIProcessor::waitCGIScript() {
		time_t now = time(NULL);
		if ((now - m_startTime) > m_timeout) {
			throw http::HttpException(http::GATEWAY_TIMEOUT, "CGI Process timed out after " + shared::string::toString(m_timeout) + " seconds");
		}

		if (hasIOError()) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "CGI Event Handler failed"); // sus
		}

		if (!isIOComplete()) {
			return true;
		}


		int status;
		pid_t result = waitpid(m_pid, &status, WNOHANG);
		if (result == -1) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "waitpid() failed " + std::string(std::strerror(errno)));
		} else if (result == 0) {
			return true;
		}

		LOG_INFO("CGI Process exited with status code: " + shared::string::toString(status));
		m_pid = -1;

		return false;
	}

	bool CGIProcessor::isIOComplete() const { return (m_ioState & IO_READ_COMPLETE) != 0 && (m_ioState & IO_WRITE_COMPLETE) != 0; }

	bool CGIProcessor::hasIOError() const { return (m_ioState & IO_ERROR) != 0; }

	void CGIProcessor::setenv(const char* name, const char* value) const {
		if (::setenv(name, value, true)) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "setenv() failed" + std::string(std::strerror(errno)));
		}
	}

	void CGIProcessor::cleanup() {
		try {
			m_dispatcher.unregisterHandler(m_outputPipe.getReadFd());
			m_dispatcher.unregisterHandler(m_inputPipe.getWriteFd());
		} catch (...) {}

		delete m_response;
		m_response = NULL;

		if (m_pid != -1) {
			kill(m_pid, SIGKILL);
			m_pid = -1;
		}

		m_inputPipe.close();
		m_outputPipe.close();
	}

} /* namespace core */
