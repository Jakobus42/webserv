#include "core/CGIProcessor.hpp"

#include <signal.h>
#include <sys/wait.h>

#include "core/CGIEventHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>

extern char** environ;

// todo: add CGI timeout and interpreters paths to config

namespace core {

	const time_t CGIProcessor::DEFAULT_TIMEOUT = 30;

	CGIProcessor::CGIProcessor(io::Dispatcher& dispatcher)
		: m_dispatcher(dispatcher)
		, m_response(NULL)
		, m_pid(-1)
		, m_inputPipe()
		, m_outputPipe()
		, m_startTime(-1)
		, m_timeout(DEFAULT_TIMEOUT)
		, m_ioState(IO_NONE)
		, m_state(EXECUTE)
		, m_env(NULL) {
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
		m_ioState = IO_NONE;
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

	// todo: check if script exists before exec - this should maybe happen in router or something
	void CGIProcessor::executeCGIScript(const http::Request& request) {
		m_startTime = std::time(NULL);

		m_pid = fork();
		if (m_pid == -1) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "fork() failed " + std::string(std::strerror(errno)));
		}

		if (m_pid == 0) {
			try {
				m_dispatcher.close(); // super sus

				m_inputPipe.dupReadFd(STDIN_FILENO);
				m_inputPipe.close();
				m_outputPipe.dupWriteFd(STDOUT_FILENO);
				m_outputPipe.close();

				// todo: try to throw here - might cause issues because epoll fds are already closed

				prepareEnviorment(request);

				const std::string& scriptPath = request.getUri().getPath();
				const std::string& interpreter = getInterpreter(scriptPath);
				char* const argv[] = {
					const_cast<char*>(interpreter.c_str()),
					const_cast<char*>(scriptPath.c_str()),
					NULL};

				execve(interpreter.c_str(), argv, m_env);
				throw std::runtime_error("execve() failed: " + std::string(std::strerror(errno)));
			} catch (const std::exception& e) {
				LOG_ERROR("child process: " + std::string(e.what()));
				std::exit(EXIT_FAILURE);
			}
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
				m_inputPipe.closeWriteEnd();
			}
		}
	}

	// todo: get real interpreter path from env or config?
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

	// todo: maybe add some mor stuff (need more context for that)
	void CGIProcessor::prepareEnviorment(const http::Request& request) {
		std::vector<std::string> envVars;

		envVars.push_back("PATH_INFO=" + request.getUri().getCgiPathInfo());
		envVars.push_back("SERVER_PROTOCOL=" + request.getVersion());
		envVars.push_back("QUERY_STRING=" + request.getUri().getQuery());
		envVars.push_back("REQUEST_METHOD=" + std::string(methodToString(request.getMethod())));
		envVars.push_back("SCRIPT_NAME=" + request.getUri().getPath().substr(9 /* /cgi-bin/ */));
		envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");

		if (request.hasHeader("content-length")) {
			envVars.push_back("CONTENT_LENGTH=" + request.getHeader("content-length").front());
		} else {
			envVars.push_back("CONTENT_LENGTH=0");
		}

		if (request.hasHeader("content-type")) {
			envVars.push_back("CONTENT_TYPE=" + request.getHeader("content-type").front());
		} else {
			envVars.push_back("CONTENT_TYPE=text/plain");
		}

		for (http::Request::HeaderMap::const_iterator it = request.getHeaders().begin();
			 it != request.getHeaders().end();
			 ++it) {
			std::string headerName = "HTTP_" + it->first;

			for (size_t i = 0; i < headerName.length(); ++i) {
				if (headerName[i] == '-') {
					headerName[i] = '_';
				} else {
					headerName[i] = std::toupper(headerName[i]);
				}
			}

			envVars.push_back(headerName + "=" + shared::string::join(it->second, ", "));
		}

		for (std::size_t i = 0; environ[i] != NULL; ++i) {
			std::string entry(environ[i]);
			if (std::find(envVars.begin(), envVars.end(), entry) == envVars.end()) {
				envVars.push_back(entry);
			}
		}

		// todo: malloc fail
		m_env = new char*[envVars.size() + 1];
		for (std::size_t i = 0; i < envVars.size(); ++i) {
			m_env[i] = new char[envVars[i].length() + 1];
			std::copy(envVars[i].c_str(),
					  envVars[i].c_str() + envVars[i].length() + 1,
					  m_env[i]);
		}

		m_env[envVars.size()] = NULL;
	}

	bool CGIProcessor::waitCGIScript() {
		time_t now = time(NULL);
		if ((now - m_startTime) > m_timeout) {
			throw http::HttpException(http::GATEWAY_TIMEOUT, "CGI Process timed out after " + shared::string::toString(m_timeout) + " seconds");
		}

		if (hasIOError()) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "CGI Event Handler failed");
		}

		int status;
		pid_t result = waitpid(m_pid, &status, WNOHANG);
		if (result == -1) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "waitpid() failed " + std::string(std::strerror(errno)));
		} else if (result == 0) {
			return true;
		}

		m_pid = -1;

		if (WIFEXITED(status)) {
			int exitCode = WEXITSTATUS(status);
			if (exitCode != 0) {
				throw http::HttpException(http::BAD_GATEWAY,
										  "CGI script exited with non-zero exit code: " + shared::string::toString(exitCode));
			}
		} else if (WIFSIGNALED(status)) {
			throw http::HttpException(http::BAD_GATEWAY,
									  "CGI script terminated by signal: " + shared::string::toString(WTERMSIG(status)));
		}

		if (!isIOComplete()) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "CGI script finished but io handlers are not complete");
		}

		return false;
	}

	bool CGIProcessor::isIOComplete() const { return (m_ioState & IO_READ_COMPLETE) != 0 && (m_ioState & IO_WRITE_COMPLETE) != 0; }

	bool CGIProcessor::hasIOError() const { return (m_ioState & IO_ERROR) != 0; }

	void CGIProcessor::cleanup() {
		try {
			m_dispatcher.unregisterHandler(m_outputPipe.getReadFd());
			if (m_inputPipe.getWriteFd() != -1) {
				m_dispatcher.unregisterHandler(m_inputPipe.getWriteFd());
			}
		} catch (...) {}

		delete m_response;
		m_response = NULL;

		if (m_pid != -1) {
			kill(m_pid, SIGKILL);
			m_pid = -1;
		}

		m_inputPipe.close();
		m_outputPipe.close();

		if (m_env) {
			for (std::size_t i = 0; m_env[i]; ++i) {
				delete m_env[i];
			}
			delete m_env;
			m_env = NULL;
		}
	}

} /* namespace core */
