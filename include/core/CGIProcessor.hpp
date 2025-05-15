#pragma once

#include "config/ServerConfig.hpp"
#include "http/http.hpp"
#include "io/Dispatcher.hpp"
#include "io/Pipe.hpp"
#include "shared/NonCopyable.hpp"

namespace http {
	class Request;
	class Response;
} // namespace http

namespace core {

	class CGIProcessor : shared::mixin::NonCopyable {
		public:
			explicit CGIProcessor(io::Dispatcher& dispatcher, const config::ServerConfig& serverConfig);
			~CGIProcessor();

			bool process(const http::Request& request);
			http::Response* releaseResponse();

			void reset();

			// .-. hacky shit
			void notifyIOReadCompletion();
			void notifyIOWriteCompletion();
			void notifyIOError(http::StatusCode statusCode, const std::string& reason);

		private:
			enum State {
				EXECUTE,
				WAIT,
				DONE
			};

			enum IOState {
				IO_NONE = 0x0,
				IO_READ_COMPLETE = 0x01,
				IO_WRITE_COMPLETE = 0x02,
				IO_ERROR = 0x04
			};

			void prepareEnviorment(const http::Request& request);
			void executeCGIScript(const http::Request& request);
			const std::string& getInterpreter();

			bool monitor();
			bool waitCGIScript();

			bool isIOComplete() const;
			bool isIOReadComplete() const;
			bool isIOWriteComplete() const;
			bool hasIOError() const;
			void cleanup();

		private:
			io::Dispatcher& m_dispatcher;
			const config::ServerConfig& m_serverConfig;

			http::Response* m_response;

			pid_t m_pid;
			io::Pipe m_inputPipe;
			io::Pipe m_outputPipe;
			time_t m_startTime;
			time_t m_timeout;
			int32_t m_ioState;
			std::string m_lastIOErrorReason;
			http::StatusCode m_lastIOStatusCode;
			std::string m_scriptName;
			State m_state;

			char** m_env;
	};

} /* namespace core */
