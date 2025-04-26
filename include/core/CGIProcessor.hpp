#pragma once

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
			explicit CGIProcessor(io::Dispatcher& dispatcher);
			~CGIProcessor();

			bool process(const http::Request& request);
			http::Response* releaseResponse();

			void reset();

		private:
			enum State {
				EXECUTE,
				WAIT,
				DONE
			};

			void prepareEnviorment(const http::Request& request);
			void executeCGIScript(const http::Request& request);
			const std::string& getInterpreter(const std::string& scriptPath);


			void setenv(const char* name, const char* value) const;

			void cleanup();

		private:
			io::Dispatcher& m_dispatcher;

			http::Response* m_response;

			pid_t m_pid;
			io::Pipe m_inputPipe;
			io::Pipe m_outputPipe;

			State m_state;
	};

} /* namespace core */
