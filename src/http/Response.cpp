#include "http/Response.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: AMessage()
		, m_statusCode(OK)
		, m_reasonPhrase(statusCodeToMessage(m_statusCode)) {
	}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	StatusCode Response::getStatusCode() const { return m_statusCode; }

	const std::string& Response::getReasonPhrase() const { return m_reasonPhrase; }

	void Response::setStatusCode(StatusCode code) { m_statusCode = code; }

	void Response::setReasonPhrase(const std::string& phrase) { m_reasonPhrase = phrase; }

	void Response::setReasonPhrase(const shared::string::StringView& phrase) { m_reasonPhrase.assign(phrase.begin(), phrase.size()); }

} /* namespace http */
