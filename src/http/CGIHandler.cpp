#include "http/CGIHandler.hpp"

#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

namespace http {

	/**
	 * @brief Constructs a new CGIHandler object.
	 */
	CGIHandler::CGIHandler(Router& router)
		: ARequestHandler(router)
		, m_pid(-1)
		, m_stdin()
		, m_stdout() {}

	/**
	 * @brief Destroys the CGIHandler object.
	 */
	CGIHandler::~CGIHandler() {
		this->reset();
	}

	void CGIHandler::handle(const Request& req, Response& res) {
		switch (m_state) {
			case PENDING:
				this->launchScript(req);
				/* fallthrough */
			case PROCESSING:
				this->collectScriptOutput(req, res);
			case DONE:
				break;
		}
	}

	void CGIHandler::reset() {
		this->ARequestHandler::reset();
		m_stdin.close();
		m_stdout.close();
		this->killProcess();
	}

	void CGIHandler::launchScript(const Request& req) {
		this->prepareEnviroment(req);

		
		m_stdin.create();
		m_stdout.create();
		
		m_pid = fork();
		if (m_pid == -1) {
			throw http::exception(INTERNAL_SERVER_ERROR, "Failed to fork CGI process: " + shared::string::to_string(strerror(errno)));
		}

		if (m_pid == 0) {
			m_stdin.redirectRead(STDIN_FILENO);
			m_stdin.close();

			m_stdout.redirectWrite(STDOUT_FILENO);
			m_stdout.close();

			const std::string& path = req.getUri().safeAbsolutePath;
			const std::string& interpreter = this->getInterpreter(path);
			char* argv[] = {const_cast<char*>(interpreter.c_str()), const_cast<char*>(path.c_str()), NULL};

			execve(interpreter.c_str(), reinterpret_cast<char* const*>(argv), environ);
			LOG("execve() failed: " + shared::string::to_string(strerror(errno)), shared::ERROR);
			exit(1);
		}
		m_stdout.close();
		m_stdin.close();
		m_state = PROCESSING;
	}

	const std::string& CGIHandler::getInterpreter(const std::string& scriptPath) {
		size_t dotPos = scriptPath.find_last_of('.');
		std::string extension = (dotPos != std::string::npos) ? scriptPath.substr(dotPos) : "";

		static std::map<std::string, std::string> interpreters;
		if (interpreters.empty()) {
			interpreters[".py"] = "python3";
		}

		std::map<std::string, std::string>::const_iterator it = interpreters.find(extension);
		if (it != interpreters.end()) {
			return it->second;
		}

		throw http::exception(BAD_REQUEST, "Unsupported script type: " + extension);
	}

	void CGIHandler::prepareEnviroment(const Request& req) const {
		this->setenv("PATH_INFO", req.getUri().cgiPathInfo.c_str());
		this->setenv("SCRIPT_NAME", req.getUri().path.c_str());
		this->setenv("QUERY_STRING", req.getUri().query.c_str());
	}

	void CGIHandler::setenv(const char* name, const char* value) const {
		if (::setenv(name, value, true)) {
			throw http::exception(INTERNAL_SERVER_ERROR, "setenv() failed");
		}
	}

	void CGIHandler::collectScriptOutput(const Request&, Response&) {
		int32_t status;
		if(waitpid(m_pid, &status, WNOHANG) == -1) {
			throw http::exception(INTERNAL_SERVER_ERROR, "waitpid() failed");
		}
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			m_state = DONE;
        }
	}

	void CGIHandler::killProcess() {
		if (m_pid != -1) {
			kill(m_pid, SIGKILL);
			m_pid = -1;
		}
	}

	CGIHandler::Pipe::Pipe()
		: m_read_fd(-1)
		, m_write_fd(-1) {}

	CGIHandler::Pipe::~Pipe() { this->close(); }

	void CGIHandler::Pipe::create() {
		int32_t fd[2];

		if (pipe(fd) == -1) {
			throw http::exception(INTERNAL_SERVER_ERROR, "pipe() failed " + shared::string::to_string(strerror(errno)));
		}
		m_read_fd = fd[0];
		m_write_fd = fd[1];
	}

	void CGIHandler::Pipe::close() {
		this->closeRead();
		this->closeWrite();
	}

	void CGIHandler::Pipe::closeRead() { this->safeClose(&m_read_fd); }

	void CGIHandler::Pipe::closeWrite() { this->safeClose(&m_write_fd); }

	void CGIHandler::Pipe::redirectRead(int32_t to_fd) const { this->dup2(m_read_fd, to_fd); }

	void CGIHandler::Pipe::redirectWrite(int32_t to_fd) const { this->dup2(m_write_fd, to_fd); }

	int32_t CGIHandler::Pipe::getReadFd() const { return m_read_fd; }

	int32_t CGIHandler::Pipe::getWriteFd() const { return m_write_fd; }

	void CGIHandler::Pipe::safeClose(int32_t* fd) {
		if (*fd != -1) {
			if (::close(*fd)) {
				throw http::exception(INTERNAL_SERVER_ERROR, "close() failed " + shared::string::to_string(strerror(errno)));
			}
			*fd = -1;
		}
	}

	void CGIHandler::Pipe::dup2(int32_t fd1, int32_t fd2) {
		if (::dup2(fd1, fd2) == -1) {
			throw http::exception(INTERNAL_SERVER_ERROR, "dup2() failed " + shared::string::to_string(strerror(errno)));
		}
	}

	uint32_t CGIHandler::Pipe::read(char* buffer, size_t bufferSize) {
		ssize_t bytesRead = ::read(m_read_fd, buffer, bufferSize);
		if (bytesRead == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return 0;
			}
			throw http::exception(INTERNAL_SERVER_ERROR, "read() failed: " + shared::string::to_string(strerror(errno)));
		}
		return bytesRead;
	}

	uint32_t CGIHandler::Pipe::write(char* data, size_t size) {
		ssize_t bytesWritten = ::write(m_write_fd, data, size);
		if (bytesWritten == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return 0;
			}
			throw http::exception(INTERNAL_SERVER_ERROR, "write() failed: " + shared::string::to_string(strerror(errno)));
		}
		return bytesWritten;
	}

} /* namespace http */
