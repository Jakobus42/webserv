#pragma once

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
				, m_filePath("") {}

			virtual ~ARequestHandler() {}

			virtual bool handle(const http::Request& request, http::Response& response) = 0;

			virtual void reset() {
				m_state = PREPROCESS;
				m_filePath.clear();
			}

			void setFilePath(const std::string& filePath) { m_filePath = filePath; }

		protected:
			enum HandlerState {
				PREPROCESS,
				PROCESS,
				DONE
			};

		protected:
			HandlerState m_state;
			std::string m_filePath;
	};

} /* namespace core */
