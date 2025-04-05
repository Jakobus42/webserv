#include "http/Response.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: AMessage()
		, m_statusCode(http::OK)
		, m_reasonPhrase(http::statusCodeToMessage(m_statusCode)) {
	}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	http::StatusCode Response::getStatusCode() const { return m_statusCode; }

	const std::string& Response::getReasonPhrase() const { return m_reasonPhrase; }

	void Response::setStatusCode(http::StatusCode code) { m_statusCode = code; }

	void Response::setReasonPhrase(const std::string& phrase) { m_reasonPhrase = phrase; }

	void Response::setReasonPhrase(const shared::StringView& phrase) { m_reasonPhrase.assign(phrase.begin(), phrase.size()); }

} /* namespace http */
