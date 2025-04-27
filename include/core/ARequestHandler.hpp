#pragma once

#include "shared/NonCopyable.hpp"

namespace http {
	class Request;
	class Response;
} // namespace http

namespace core {

	class ARequestHandler {
		public:
			ARequestHandler()
				: m_state(PREPROCESS) {}

			virtual ~ARequestHandler() {}

			virtual bool handle(const http::Request& request, http::Response& response) = 0;

			virtual void reset() { m_state = PREPROCESS; }

		protected:
			enum HandlerState {
				PREPROCESS,
				PROCESS,
				DONE
			};

		protected:
			HandlerState m_state;
	};

} /* namespace core */
