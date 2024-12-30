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

			void setRawResponse(const std::string&); // probably just temporary
			const std::string& getRawResponse(void) const;
			ResponseBuilderStatus getStatus(void) const;
			StatusCode getStatusCode(void) const;

			void reset(void);

		private:
			std::string m_rawResponse;
			StatusCode m_statusCode;

			ResponseBuilderStatus m_status;
	};

} /* namespace http */
