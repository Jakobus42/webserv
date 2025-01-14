#include "http/Response.hpp"

#include "http/Request.hpp"
#include "http/http.hpp"

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

	// temporarily always error to test error response
	void Response::doMagicToCalculateStatusCode(const Request& request) {
		(void)request;
		m_statusCode = NOT_FOUND;
		m_type = ERROR;
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

		this->m_body = getErrorPage(m_statusCode);

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

} /* namespace http */
