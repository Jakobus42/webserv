#pragma once

#include "shared/defines.hpp"

#include <map>
#include <string>

namespace http {

	class Request;

	typedef std::map<std::string, std::string> t_headerFields;

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

			ResponseBuilderStatus getBuilderStatus(void) const;
			const std::string& getRawResponse(void) const;
			StatusCode getStatusCode(void) const;
			const std::string& getStatusLine(void) const;
			const t_headerFields& getHeaderFields(void) const;
			const std::string& getBody(void) const;

			void setRawResponse(const std::string&); // probably just temporary

			void buildFromRequest(const http::Request&);
			bool done(void) const;
			void reset(void);

		private:
			std::string m_rawResponse;
			StatusCode m_statusCode;
			std::string m_statusLine;
			t_headerFields m_headerFields;
			std::string m_body;

			ResponseBuilderStatus m_builderStatus;
	};

} /* namespace http */
