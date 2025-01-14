#include "http/Response.hpp"

#include "http/Request.hpp"
#include "http/constants.hpp"

#include <ctime>
#include <iomanip>

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: m_type(IDK_NORMAL_I_GUESS)
		, m_rawResponse("")
		, m_statusCode(UNKNWN)
		, m_headerFields()
		, m_body()
		, m_pathData()
		, m_builderStatus(PENDING_WRITE) {}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Response object to copy.
	 */
	Response::Response(const Response& other)
		: m_type(other.getType())
		, m_rawResponse(other.getRawResponse())
		, m_statusCode(other.getStatusCode())
		, m_headerFields(other.getHeaderFields())
		, m_body(other.getBody())
		, m_pathData(other.getPathData())
		, m_builderStatus(other.getBuilderStatus()) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Response object to assign from.
	 * @return A reference to the assigned Response object.
	 */
	Response& Response::operator=(const Response& rhs) {
		if (this == &rhs)
			return *this;
		m_type = rhs.getType();
		m_rawResponse = rhs.getRawResponse();
		m_statusCode = rhs.getStatusCode();
		m_headerFields = rhs.getHeaderFields();
		m_body = rhs.getBody();
		m_builderStatus = rhs.getBuilderStatus();
		m_pathData = rhs.getPathData();
		return *this;
	}

	ResponseBuilderStatus Response::getBuilderStatus(void) const {
		return m_builderStatus;
	}

	ResponseType Response::getType(void) const {
		return m_type;
	}

	const std::string& Response::getRawResponse(void) const {
		return m_rawResponse;
	}

	StatusCode Response::getStatusCode(void) const {
		return m_statusCode;
	}

	const t_headerFields& Response::getHeaderFields(void) const {
		return m_headerFields;
	}

	const std::string& Response::getBody(void) const {
		return m_body;
	}

	void Response::setRawResponse(const std::string& to) {
		m_rawResponse = to;
	}

	const t_PathData Response::getPathData(void) const {
		return m_pathData;
	}

	// TODO: implement
	bool Response::done(void) const {
		return true;
	}

	inline std::ostream& Response::statusLineString(std::ostream& o) {
		std::cout << "creating status line string" << std::endl;
		std::string message;
		try {
			message = getStatusMessage(m_statusCode);
		} catch (std::exception& e) {
			std::cerr << "statusLineString failed: " << e.what() << std::endl;
			message = "fuck";
		}
		return o << HTTP_NAME << ONE_DOT_ONE << ' ' << m_statusCode << ' ' << message << CRLF; // << statusMessage at the end
	}

	inline std::ostream& Response::dateString(std::ostream& o) {
		char buffer[64] = "";
		std::time_t now = std::time(NULL);
		std::tm* gmtTime = std::gmtime(&now);
		std::strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT", gmtTime);
		return o << buffer << CRLF;
	}

	inline std::ostream& Response::serverString(std::ostream& o) {
		return o << "Server: Very Epic Gamer Server/0.1 (Powered by Hopes and Dreams)" << CRLF;
	}

	// put all the headers in the stream, formatted properly
	std::ostream& Response::headersString(std::ostream& o) {
		for (t_headerFields::iterator it = m_headerFields.begin(); it != m_headerFields.end(); ++it) {
			o << it->first << ": " << it->second << CRLF;
		}
		return o;
	}

	void Response::doMagicToCalculateStatusCode(const Request& request) {
		m_statusCode = OK;
		m_type = IDK_NORMAL_I_GUESS;
		checkRequestData(request);
		if (m_type == ERROR) {
			return;
		}
	}

	void Response::buildFromRequest(const Request& request) {
		std::stringstream ss("");
		(void)request; // process the request

		m_headerFields.insert(std::make_pair("Content-Type", "text/plain"));
		m_headerFields.insert(std::make_pair("Content-Length", "13"));
		m_statusCode = OK;
		m_body = "Hello, world!";
		this->statusLineString(ss);
		this->serverString(ss);
		this->dateString(ss);
		std::cout << "done" << std::endl;
		this->headersString(ss);
		ss << CRLF;
		if (!m_body.empty()) {
			ss << m_body;
		}
		m_rawResponse = ss.str();
		std::cout << "buildFromRequest built: " << m_rawResponse << std::endl;
	}

	void Response::buildCGIResponse(const Request& request) {
		(void)request;
	}

	void Response::buildErrorResponse(const Request& request) {
		(void)request;

		this->m_body = generateErrorPage(m_statusCode);

		std::stringstream ss("");

		this->statusLineString(ss);
		this->serverString(ss);
		this->dateString(ss);

		std::stringstream stupid;
		stupid << m_body.length();
		std::string stupider = stupid.str();
		m_headerFields.insert(std::make_pair("Content-Type", "text/html"));
		m_headerFields.insert(std::make_pair("Content-Length", stupider.c_str()));
		this->headersString(ss);
		ss << CRLF;
		if (!m_body.empty()) {
			ss << m_body;
		}
		m_rawResponse = ss.str();
	}

	void Response::reset(void) {
		m_type = IDK_NORMAL_I_GUESS;
		m_rawResponse.clear();
		m_statusCode = UNKNWN;
		m_headerFields.clear();
		m_body.clear();
		m_builderStatus = PENDING_WRITE;
	}

	/**
	 * @brief Parses Absolute path and saves only the pure path in "m_truePath" src: (RFC 9112 3.2.2)
	 * @details 
	 * absolute-URI = scheme ":" hier-part [ "?" query ]
	 * @param path 
	 */
	bool Response::parseAbsoluteForm(const std::string &path, const Request& request, t_PathData &pathData)
	{
		(void)request;
		std::string scheme;
		std::string query = "";
		std::string authority;
		std::string pure_path;
		if (path.find(':') == std::string::npos) {
			return false;
		}
		scheme = path.substr(0, path.find(":")); // scheme before the first colon
		if (scheme == "" || std::isalpha(scheme[0]) == 0
		|| scheme.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-.") != std::string::npos) {
			return false;
		}
		if (path.find('?') != std::string::npos) { // optional query part after the first question mark
			query = path.substr(path.find('?') +1);
		}
		pure_path = path.substr(path.find(':') +1, path.find('?') - path.find(':') -1);
		if (pure_path[0] != '/' && pure_path[1] != '/') { // authority part starts with two slashes
			return false;

		}
		pure_path = pure_path.substr(2);
		if (pure_path.find('/') == std::string::npos) {
			return false;
		}
		authority = pure_path.substr(0, pure_path.find('/'));
		if (authority.empty()) {
			return false;
		}
		pure_path = pure_path.substr(pure_path.find('/'));
		pathData.query = query;
		pathData.pure_path = pure_path;
		pathData.scheme = scheme;
		pathData.authority = authority;
		return true;
	}

	bool  Response::parseOriginForm(const std::string &path, const Request& request, t_PathData &pathData)
	{
		std::string authority;
		std::string pure_path;
		std::string query;
		if (path.find('?') != std::string::npos) { // optional query part after the first question mark
			query = path.substr(path.find('?') +1);	
		}
		pure_path = path.substr(0, path.find('?'));
		if (pure_path[0] != '/') {
			return false;
		}
		int found = 0;
		for (std::vector<t_header>::const_iterator it = request.getRequestData().headers.begin(); it != request.getRequestData().headers.end(); ++it)
		{
			if (it->first == "Host") {
				if (found == 1) {
					return false;
				}
				if (it->second.size() != 1) {
					return false;
				}
				authority = it->second[0];
				found = 1;
			}
		}
		if (found == 0) {
			return false;
		}
		pathData.query = query;
		pathData.pure_path = pure_path;
		pathData.authority = authority;
		pathData.scheme = "";
		return true;
	}

	/**
	 * @brief find request target form, check it and reconstruct it (RFC 9112 3.2)
	 * @details has to be either "Origin-form" or "Absolute-form" (others only with unsupported methods)
	 * @param request 
	 */
	void Response::checkAndReconstructTargetUri(const Request& request)
	{
		(void)request;
		bool ret = false;
		if (request.getRequestData().uri[0] == '/') {
			ret = parseOriginForm(request.getRequestData().uri, request, m_pathData);
		} else {
			ret = parseAbsoluteForm(request.getRequestData().uri, request, m_pathData);
		}
		if (!ret) {
			m_statusCode = BAD_REQUEST;
			m_type = ERROR;
		}
	}

	/**
	 * @brief Check if the request data is valid, and set the status code accordingly
	 * @details Stuff being checked:
	 * - Method is GET, POST or DELETE
	 * - Uri is valid, in the correct form and gets reconstructed
	 * - Version is HTTP/1.1
	 * @param request The request to check
	 */
	void Response::checkRequestData(const Request& request)
	{
		if (request.getRequestData().method != "GET" && request.getRequestData().method != "POST" && request.getRequestData().method != "DELETE") {
			m_statusCode = NOT_IMPLEMENTED;
			m_type = ERROR;
			return;
		}
		if (request.getRequestData().uri.length() > 8192) {
			m_statusCode = URI_TOO_LONG;
			m_type = ERROR;
			return;
		}
		checkAndReconstructTargetUri(request);
		if (m_type == ERROR) {
			return;
		}
		if (request.getRequestData().version != "HTTP/1.1") {
			m_statusCode = HTTP_VERSION_NOT_SUPPORTED;
			m_type = ERROR;
			return;
		}
		// (RFC 9112 3.2)
/* 		if (request.getRequestData().headers.find("Host") == request.getRequestData().headers.end() 
			|| request.getRequestData().headers.find("Host")->second.size() != 1) {
			m_statusCode = BAD_REQUEST;
			m_type = ERROR;
			return;
		} */
	}

	int Response::testParseURI(const std::string& uri, int mode)
	{
		std::cout << "-----------------------------------" << std::endl;
		t_PathData pathData;
		if (mode == 0) {
			Request req;
			std::vector<std::string> host;
			host.push_back("localhost");
			req.setHeader("Host", host);
			if (parseOriginForm(uri, req, pathData)) {
				std::cout << "Origin form parsed successfully" << std::endl;
			} else {
				std::cout << "Origin form failed" << std::endl;
				std::cout << "-----------------------------------" << std::endl;
				return 1;
			}
		} else {
			if (parseAbsoluteForm(uri, Request(), pathData)) {
				std::cout << "Absolute form parsed successfully" << std::endl;
			} else {
				std::cout << "Absolute form failed" << std::endl;
				std::cout << "-----------------------------------" << std::endl;
				return 1;
			}
		}
		std::cout << "Scheme: " << pathData.scheme << std::endl;
		std::cout << "Authority: " << pathData.authority << std::endl;
		std::cout << "Pure path: " << pathData.pure_path << std::endl;
		std::cout << "Query: " << pathData.query << std::endl;
		std::cout << "-----------------------------------" << std::endl;
		return 0;
	}


} /* namespace http */
