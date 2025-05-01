#pragma once

#include "core/Router.hpp"
#include "shared/NonCopyable.hpp"

#include <string>

namespace http {
	class Request;
	class Response;
} // namespace http

namespace core {

	class ARequestHandler {
		public:
			ARequestHandler()
				: m_state(PREPROCESS)
				, m_absoluteFilePath("")
				, m_route() {}

			virtual ~ARequestHandler() {}

			virtual bool handle(const http::Request& request, http::Response& response) = 0;

			virtual void reset() {
				m_state = PREPROCESS;
				m_absoluteFilePath.clear();
				m_route.reset();
			}

			bool needsRoute() const { return m_route.location == NULL; }

			void setRoute(const Route& route) { m_route = route; }

		protected:
			enum HandlerState {
				PREPROCESS,
				PROCESS,
				DONE
			};

		protected:
			HandlerState m_state;
			std::string m_absoluteFilePath;
			Route m_route;
	};

} /* namespace core */
