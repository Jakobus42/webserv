#pragma once

#include "shared/defines.hpp"

#include <string>

namespace http {

	enum ResponseBuilderStatus {
		PENDING_WRITE,
		WRITING,
		PENDING_SEND,
		SENT
	};

	/**
	 * @class Response
	 * @brief ...
	 */
	class Response {
		public:
			Response();
			~Response();
			Response(const Response& other);
			Response& operator=(const Response& rhs);

			const std::string& getRawResponse(void) const;
			StatusCode getStatusCode(void) const;

		private:
			std::string m_rawResponse;
			StatusCode m_statusCode;

			ResponseBuilderStatus m_status;
	};

} /* namespace http */
