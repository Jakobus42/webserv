#include "core/CGIProcessor.hpp"

#include <signal.h>

#include "core/CGIEventHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"

#include <cstdlib>
#include <cstring>

namespace core {

	CGIProcessor::CGIProcessor(io::Dispatcher& dispatcher)
		: m_dispatcher(dispatcher)
		, m_response(NULL)
		, m_pid(-1)
		, m_inputPipe()
		, m_outputPipe()
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
				prepareEnviorment(request);
				executeCGIScript(request); // todo: somehow get the corret path
				m_state = WAIT;
				break;
			}
			case WAIT: break;
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

	// todo: exit child properly
	// todo: check if script exists before exec - this should maybe happen in router or something
	void CGIProcessor::executeCGIScript(const http::Request& request) {
		m_pid = fork();
		if (m_pid == -1) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "fork() failed " + std::string(std::strerror(errno)));
		}

		if (m_pid == 0) {
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

			m_dispatcher.registerHandler(m_outputPipe.getReadFd(), new CGIEventHandler(), io::AMultiplexer::EVENT_READ);
			if (request.getMethod() == http::POST) {
				m_dispatcher.registerHandler(m_inputPipe.getWriteFd(), new CGIEventHandler(), io::AMultiplexer::EVENT_WRITE);
			}
		}
	}

	// todo: get real path from env
	const std::string& CGIProcessor::getInterpreter(const std::string& scriptPath) {
		size_t dotPos = scriptPath.find_last_of('.');
		std::string extension = (dotPos != std::string::npos) ? scriptPath.substr(dotPos) : "";

		static std::map<std::string, std::string> interpreters;
		if (interpreters.empty()) {
			interpreters[".py"] = "/usr/bin/python3";
		}

		std::map<std::string, std::string>::const_iterator it = interpreters.find(extension);
		if (it != interpreters.end()) {
			return it->second;
		}

		throw http::HttpException(http::NOT_IMPLEMENTED, "unsupported script type: " + extension);
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
