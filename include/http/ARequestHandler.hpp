#pragma once

#include "shared/NonCopyable.hpp"

namespace http {

	class Request;
	class Response;

	class ARequestHandler {
		public:
			ARequestHandler()
				: m_state(PREPROCESS) {}

			~ARequestHandler() {}

			virtual void handle(const Request* request, Response* response) = 0;

			virtual void reset() { m_state = PREPROCESS; }

			bool isDone() { return m_state == DONE; }

		protected:
			enum HandlerState {
				PREPROCESS,
				PROCESS,
				DONE
			};

		protected:
			HandlerState m_state;
	};

} /* namespace http */
