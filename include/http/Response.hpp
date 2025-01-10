#pragma once

#include "http/constants.hpp"

#include <map>
#include <ostream>
#include <string>

const std::string HTTP_NAME = "HTTP/";
const std::string ONE_DOT_ONE = "1.1";
const std::string CRLF = "\r\n";

namespace http {

	class Request;

	typedef std::map<std::string, std::string> t_headerFields;

	typedef struct s_PathData {
		std::string scheme;
		std::string authority;
		std::string pure_path;
		std::string query;
	} t_PathData;

	enum ResponseBuilderStatus {
		PENDING_WRITE,
		WRITING,
		PENDING_SEND,
		SENT
	};

	enum ResponseType {
		IDK_NORMAL_I_GUESS,
		CGI,
		ERROR
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
			ResponseType getType(void) const;
			const std::string& getRawResponse(void) const;
			StatusCode getStatusCode(void) const;
			const t_headerFields& getHeaderFields(void) const;
			const std::string& getBody(void) const;
			const t_PathData getPathData(void) const;

			void setRawResponse(const std::string&); // probably just temporary
			void doMagicToCalculateStatusCode(const Request&);

			std::ostream& headersString(std::ostream& o);
			inline std::ostream& statusLineString(std::ostream& o);
			inline std::ostream& dateString(std::ostream& o);
			inline std::ostream& serverString(std::ostream& o);

			void buildFromRequest(const http::Request&);
			void buildCGIResponse(const http::Request&);
			void buildErrorResponse(const http::Request&);
			bool done(void) const;
			void reset(void);

		private:
			ResponseType m_type;
			std::string m_rawResponse;
			StatusCode m_statusCode;
			t_headerFields m_headerFields;
			std::string m_body;
			t_PathData m_pathData;
			
			ResponseBuilderStatus m_builderStatus;

			void checkRequestData(const Request &request);
			void checkAndReconstructTargetUri(const Request &request);
			bool parseAbsoluteForm(const std::string &path, const Request& request, t_PathData &pathData);
			bool parseOriginForm(const std::string &path, const Request& request, t_PathData &pathData);
	};

} /* namespace http */
