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
			StatusCode getStatusCode() const;
			const std::string& getReasonPhrase() const;

			void setStatusCode(StatusCode code);
			void setReasonPhrase(const std::string& phrase);
			void setReasonPhrase(const shared::StringView& phrase);

		private:
			StatusCode m_statusCode;
			std::string m_reasonPhrase;
	};

} /* namespace http */
