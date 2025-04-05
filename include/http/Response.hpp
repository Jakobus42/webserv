#pragma once

#include "http/AMessage.hpp"
#include "http/http.hpp"

namespace http {

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

} /* namespace http */
