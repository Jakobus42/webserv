#pragma once

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Router.hpp"
#include "shared/Buffer.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/stringUtils.hpp"

namespace http {

	/**
	 * @class ARequestHandler
	 * @brief ...
	 */
	class ARequestHandler : shared::NonCopyable {
		public:
			enum HandlerState {
				PENDING,
				PROCESSING,
				DONE
			};

			ARequestHandler(Router& router)
				: m_state(PENDING)
				, m_filePath("")
				, m_router(router) {}

			virtual ~ARequestHandler() {}

			virtual void handle(const Request& request, Response& response) = 0;

			virtual void reset() {
				m_state = PENDING;
				m_filePath.clear();
			}

			void handleError(Response& response) {
				// if (response.getLocation().errorPages matches response.getStatusCode()) {
				//   if file exists (matched code to errorPage) {
				//     read file and return that
				//} else {
				// proceed as usual
				//}
				//}
				response.setBody(getErrorPage(response.getStatusCode()));
				response.setHeader("Content-Length", shared::string::toString(response.getBody().length()));
				response.setHeader("Content-Type", TEXT_HTML);
			}

			bool isComplete() const {
				return m_state == DONE;
			}

			std::size_t getChunkSize() const { return CHUNK_SIZE; }
			HandlerState getState() const {
				return m_state;
			}

		protected:
			static const std::size_t CHUNK_SIZE = 16384; // 16 KB


		protected:
			HandlerState m_state;
			std::string m_filePath;
			Router& m_router;
	};

} /* namespace http */
