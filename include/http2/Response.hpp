#pragma once

#include "http/http.hpp"
#include "http2/AMessage.hpp"

namespace http2 {

	/**
	 * @class Response
	 * @brief ...
	 */
	class Response : public AMessage {
		public:
			Response();
			~Response();

			/* Start Line */
			http::StatusCode getStatusCode() const;
			const std::string& getReasonPhrase() const;

			void setStatusCode(http::StatusCode code);
			void setReasonPhrase(const std::string& phrase);
			void setReasonPhrase(const shared::StringView& phrase);

		private:
			http::StatusCode m_statusCode;
			std::string m_reasonPhrase;
	};

} /* namespace http2 */
