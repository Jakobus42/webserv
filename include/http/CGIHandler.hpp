#pragma once

#include "http/ARequestHandler.hpp"

// todo maybe make a pipe class

namespace http {

	/**
	 * @class CGIHandler
	 * @brief ...
	 */
	class CGIHandler : public ARequestHandler {
		public:
			CGIHandler(Router& router);
			~CGIHandler();

			void handle(const Request& request, Response& response);
			void reset();

		private:
			void launchScript(const Request& req);
			static const std::string& getInterpreter(const std::string& scriptPath);
			void prepareEnviroment(const Request& req) const;
			void setenv(const char* name, const char* value) const;

			void collectScriptOutput(const Request& req, Response& res);

			void killProcess();

		private:
			class Pipe {
				public:
					Pipe();

					~Pipe();

					void create();

					uint32_t read(char* buffer, size_t bufferSize);
					uint32_t write(char* data, size_t size);

					void close();
					void closeRead();
					void closeWrite();

					void redirectRead(int32_t to_fd) const;
					void redirectWrite(int32_t to_fd) const;

					int32_t getReadFd() const;
					int32_t getWriteFd() const;

				private:
					static void safeClose(int32_t* fd);
					static void dup2(int32_t fd1, int32_t fd2);

				private:
					int32_t m_read_fd;
					int32_t m_write_fd;
			};

		private:
			pid_t m_pid;
			Pipe m_stdin;
			Pipe m_stdout;
	};

} /* namespace http */
