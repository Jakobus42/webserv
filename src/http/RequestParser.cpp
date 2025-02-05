#include "http/RequestParser.hpp"

#include "http/Router.hpp"
#include "shared/stringUtils.hpp"

#include <algorithm>

namespace http {

	const char RequestParser::TCHAR[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ', '!', 0, '#', '$', '%', '&', '\'', 0, 0, '*', '+', 0, '-', '.', 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 0, '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, '|', 0, '~', 0}; // the formatter is shit pardon me
	const char RequestParser::WHITESPACE[] = " \t";

	// const char RequestParser::

	RequestParser::RequestParser()
		: m_req()
		, m_contentLength(0)
		, m_state(START) {
	}

	RequestParser::~RequestParser() {}

	void RequestParser::process() {
		this->clearPendingState();

		try {
			this->parse();
		} catch (const http::exception& e) {
			std::cout << "SHIT, " << e.getMessage() << std::endl;
			m_req.setStatusCode(e.getCode());
			this->setState(ERROR);
		} catch (const std::exception& e) {
			m_req.setStatusCode(INTERNAL_SERVER_ERROR);
			this->setState(ERROR);
		}
	}

	void RequestParser::parse() {
		while (this->isComplete() != true && this->isPending() != true && m_state != ERROR) {
			if (m_state == START) {
				this->parseRequestLine();
			} else if (m_state == HEADERS || m_state == TRAILING_HEADERS) {
				this->parseHeaders();
			} else if (m_state == CHUNK_SIZE) {
				this->parseChunkSize();
			} else if (m_state == BODY || m_state == CHUNK_DATA) {
				this->parseData();
			}
		}
	}

	void RequestParser::parseRequestLine() {
		char* line = this->readLine();
		if (!line) {
			return;
		}

		std::size_t size = std::strcspn(line, WHITESPACE);
		m_req.setMethod(line, size);
		line += size + 1;

		size = std::strcspn(line, WHITESPACE);
		if (size > MAX_URI_LENGTH) {
			throw http::exception(URI_TOO_LONG, "URI too large");
		}
		if (size == 0) {
			throw http::exception(NOT_FOUND, "URI is empty");
		}
		m_req.setUriRaw(line, size);

		// parse the URI
		if (m_req.getUriRaw()[0] == '/') {
			parseUriOriginForm();
		} else {
			parseUriAbsoluteForm();
		}

		line += size + 1;

		size = std::strcspn(line, WHITESPACE);
		m_req.setVersion(line, size);
		line += size;

		if (*line) {
			throw http::exception(BAD_REQUEST, "malformed request line: expected seperation of exactly 2 spaces");
		}

		m_state = HEADERS;
	}

	// void RequestParser::parseUri() {
	// 	const std::string& uriRaw = m_req.getUriRaw();
	// 	const std::size_t questionMarks = std::count(uriRaw.begin(), uriRaw.end(), '?');
	// 	const std::size_t spaces = std::count(uriRaw.begin(), uriRaw.end(), ' ');

	// 	if (questionMarks > 1 || spaces > 0) {
	// 		m_req.setStatusCode(BAD_REQUEST);
	// 		throw http::exception(BAD_REQUEST, "malformed request line: unexpected ' ' or '?'");
	// 	}

	// 	std::size_t pathBeginIndex = 0;
	// 	std::size_t queryBeginIndex = uriRaw.find_first_of("#?");
	// 	Uri& uri = m_req.getUri();

	// 	if (uriRaw.find("/cgi-bin/") == 0) {
	// 		m_req.setType(CGI);
	// 		pathBeginIndex = uriRaw.find_first_of("/#?", 9);
	// 		uri.path = uriRaw.substr(0, pathBeginIndex);
	// 		uri.cgiPathInfo = decodePercentEncodedString(uriRaw.substr(pathBeginIndex, queryBeginIndex - pathBeginIndex));
	// 	} else {
	// 		uri.path = uriRaw.substr(pathBeginIndex, queryBeginIndex);
	// 	}
	// 	uri.path = decodePercentEncodedString(uri.path);
	// 	std::cout << "Parsed path: " << uri.path << std::endl;
	// 	std::cout << "CGI Path info: " << uri.cgiPathInfo << std::endl;
	// }

	// char RequestParser::decodeCharacterFromPercentEncoding(const std::string& hex) {
	// 	if (hex.length() != 2) {
	// 		throw std::invalid_argument("Invalid percent-encoded character length");
	// 	}

	// 	int value = shared::string::toNum<int>(hex, 16);

	// 	return static_cast<char>(value);
	// }

	// std::string RequestParser::decodePercentEncodedString(const std::string& encoded) {
	// 	std::string decoded;

	// 	std::cout << "decoding " << encoded << std::endl;
	// 	for (std::string::size_type i = 0; i < encoded.length(); ++i) {
	// 		if (encoded[i] == '%' && i + 2 < encoded.length()) {
	// 			try {
	// 				decoded += decodeCharacterFromPercentEncoding(encoded.substr(i + 1, 2));
	// 				i += 2; // Skip over the percent and the two hex characters
	// 			} catch (const std::invalid_argument&) {
	// 				decoded += '%';
	// 			}
	// 		} else {
	// 			decoded += encoded[i];
	// 		}
	// 	}
	// 	std::cout << "returning decoded: '" << decoded << "'" << std::endl;
	// 	return decoded;
	// }

	void RequestParser::parseUriOriginForm() {
		const std::string& uriRaw = m_req.getUriRaw();

		http::Uri& uri = m_req.getUri();
		if (uriRaw.find('?') < uriRaw.find('#')) { // optional query part after the first question mark
			uri.query = uriRaw.substr(uriRaw.find('?') + 1);
		} else {
			uri.query = "";
		}
		uri.path = uriRaw.substr(0, uriRaw.find_first_of("?#"));
		uri.scheme = "";
		parsePath();
	}

	// parse CGI if the path starts with /cgi-bin/
	void RequestParser::parsePath() {
		Uri& uri = m_req.getUri();
		if (uri.path.find("/cgi-bin/") == 0) {
			m_req.setType(CGI);
			uri.cgiPathInfo = uri.path.substr(uri.path.find_first_of("/#?", 9));
			uri.path = uri.path.substr(0, uri.path.find_first_of("/#?", 9));
			// TODO: should never receive # or ? but idk bro
			// I'm just a silly little guy, writing my silly little code
		} else {
			uri.cgiPathInfo = ""; // should never be read in this case
		}
		uri.pathSegments = Router::splitPath(uri.path);
		// TODO: should we parse this at all?
		// also, should we check if we accept the script here?
	}

	/**
	 * @brief Parses Absolute path and saves only the pure path in "m_truePath" src: (RFC 9112 3.2.2)
	 * @details
	 * absolute-URI = scheme ":" hier-part [ "?" query ]
	 * @param path
	 */
	void RequestParser::parseUriAbsoluteForm() {
		std::string scheme;
		std::string authority;
		std::string path;

		http::Uri& uri = m_req.getUri();
		const std::string& uriRaw = m_req.getUriRaw();

		if (uriRaw.find(':') == std::string::npos) {
			throw http::exception(BAD_REQUEST, "Invalid URI");
		}
		scheme = uriRaw.substr(0, uriRaw.find(":")); // scheme before the first colon
		if (scheme.empty() || std::isalpha(scheme[0]) == 0 || scheme.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-.") != std::string::npos) {
			throw http::exception(BAD_REQUEST, "Invalid URI");
		}
		path = uriRaw.substr(uriRaw.find(':') + 1, uriRaw.find('?') - uriRaw.find(':') - 1);
		if (path[0] != '/' && path[1] != '/') { // authority part starts with two slashes
			throw http::exception(BAD_REQUEST, "Invalid URI");
		}
		path = path.substr(2);
		if (path.find('/') == std::string::npos) {
			throw http::exception(BAD_REQUEST, "Invalid URI");
		}
		authority = path.substr(0, path.find('/'));
		if (authority.empty()) {
			throw http::exception(BAD_REQUEST, "Invalid URI");
		}
		if (uriRaw.find('?') != std::string::npos) { // optional query part after the first question mark
			uri.query = uriRaw.substr(uriRaw.find('?') + 1);
		} else {
			uri.query = "";
		}
		uri.path = path.substr(path.find('/'));
		uri.scheme = scheme;	   // likely unused
		uri.authority = authority; // likely unused
		parsePath();
	}

	void RequestParser::parseHeaders() {
		while (true) {
			char* line = this->readLine();
			if (!line) {
				return;
			} else if (!*line) {
				break;
			}

			Token key = this->extractHeaderKey(line);
			if (!*line) {
				m_req.setHeader(key.token, key.size, NULL, 0);
			}
			std::size_t valCount = 0;
			while (*line) {
				Token value = this->extractHeaderValue(line);
				m_req.setHeader(key.token, key.size, value.token, value.size);
				if (++valCount > MAX_HEADER_VALUE_COUNT) {
					throw http::exception(PAYLOAD_TOO_LARGE, "header value count too large");
				}
			}
			if (m_req.getHeaders().size() > MAX_HEADER_COUNT) {
				throw http::exception(PAYLOAD_TOO_LARGE, "header count too large");
			}
		}
		if (m_state == HEADERS) {
			this->interpretHeaders();
		} else if (m_state == TRAILING_HEADERS) {
			this->setState(COMPLETE);
		}
	}

	RequestParser::Token RequestParser::extractHeaderKey(char*& line) {
		std::size_t keySize = 0;
		for (; *line && *line != ':'; ++line) {
			if (this->isTChar(*line) != true) {
				throw http::exception(BAD_REQUEST, "malformed header name: expected tchar");
			}
			*line = std::tolower(*line);
			keySize++;
		}

		if (keySize == 0) {
			throw http::exception(BAD_REQUEST, "malformed header name: expected size > 0");
		} else if (*line != ':') {
			throw http::exception(BAD_REQUEST, "malformed header name: missing colon separator");
		} else if (std::strchr(WHITESPACE, *(line - 1))) {
			throw http::exception(BAD_REQUEST, "malformed header name: no whitespace allowed before colon");
		}

		if (keySize > MAX_HEADER_NAME_LENGTH) {
			throw http::exception(PAYLOAD_TOO_LARGE, "header key too large");
		}

		return Token(line++ - keySize, keySize);
	}

	// empty values are allowed.
	RequestParser::Token RequestParser::extractHeaderValue(char*& line) {
		const char* value = line + std::strspn(line, WHITESPACE);
		std::size_t raw_size = std::strcspn(value, ",");

		std::size_t size = raw_size;
		while (size > 0 && std::strchr(WHITESPACE, value[size - 1])) {
			--size;
		}

		line = const_cast<char*>(value + raw_size);
		if (*line == ',') {
			++line;
		}

		if (size > MAX_HEADER_VALUE_LENGTH) {
			throw http::exception(PAYLOAD_TOO_LARGE, "header value length too large");
		}
		return Token(value, size);
	}

	// Host header HAS to match the authority component in absolute form
	// HTTP/1.1 mandates that Host is ALWAYS present, ALWAYS only one word long
	// and that, if the absolute form is present, the authority in it matches the host header's content
	void RequestParser::interpretHeaders() {
		if (!m_req.hasHeader("host")) {
			throw http::exception(BAD_REQUEST, "Host header is missing");
		}

		const std::vector<std::string>& hostHeader = m_req.getHeader("host");
		if (hostHeader.size() != 1) {
			throw http::exception(BAD_REQUEST, "Host header is malformed");
		}

		Uri& uri = m_req.getUri();
		if (uri.isAbsoluteForm() && hostHeader[0] != uri.authority) {
			throw http::exception(BAD_REQUEST, "Host header does not match authority specified in absolute form URI");
		}
		if (!uri.isAbsoluteForm()) { // shitty - put this somewhere else but check for only one host
			uri.authority = hostHeader[0];
		}
		if (m_req.hasHeader("content-length") && m_req.hasHeader("transfer-encoding")) {
			throw http::exception(BAD_REQUEST, "invalid header combination: cant have content-length and transfer-encoding");
		}
		if (m_req.hasHeader("content-length")) {
			this->validateContentLength();
			this->setState(m_contentLength == 0 ? COMPLETE : BODY);
		} else if (m_req.hasHeader("transfer-encoding")) {
			this->validateTransferEncoding();
			this->setState(CHUNK_SIZE);
		} else {
			this->setState(COMPLETE);
		}
	}

	void RequestParser::parseData() {
		if (m_contentLength) {
			size_t available = std::min(m_buffer.size(), m_contentLength);
			if (available == 0) {
				this->setPendingState();
				return;
			}

			m_req.appendToBody(m_buffer.getReadPos(), available);
			if (m_req.getBody().size() > MAX_BODY_SIZE) {
				throw http::exception(PAYLOAD_TOO_LARGE, "body too large");
			}
			m_buffer.consume(available);
			m_contentLength -= available;
			if (m_contentLength > 0) {
				this->setPendingState();
				return;
			}
		}

		if (m_state == BODY) {
			this->setState(COMPLETE);
		} else if (m_state == CHUNK_DATA) {
			if (m_buffer.size() < CRLF.length()) {
				this->setPendingState();
			} else {
				m_buffer.consume(CRLF.length());
				this->setState(CHUNK_SIZE);
			}
		}
	}

	void RequestParser::parseChunkSize() {
		const char* line = this->readLine();
		if (!line) {
			return;
		}

		try {
			m_contentLength = shared::string::toNum<std::size_t>(line, 16);
		} catch (const std::exception& e) {
			throw http::exception(BAD_REQUEST, "invalid chunk size: could not parse.");
		}
		if (m_contentLength > MAX_BODY_SIZE) {
			throw http::exception(PAYLOAD_TOO_LARGE, "chunk size too large");
		}

		if (m_contentLength == 0) {
			this->setState(TRAILING_HEADERS);
		} else {
			this->setState(CHUNK_DATA);
		}
	}

	char* RequestParser::readLine() {
		char* line = m_buffer.getReadPos();

		char* lineEnd = strstr(line, CRLF.c_str());
		if (lineEnd == NULL) {
			this->setPendingState();
			return NULL;
		}

		*lineEnd = '\0';
		m_buffer.consume(lineEnd + CRLF.length() - line);
		return line;
	}

	void RequestParser::validateContentLength() {
		const std::vector<std::string>& contentLengthVec = m_req.getHeader("content-length");
		if (contentLengthVec.size() != 1) {
			throw http::exception(BAD_REQUEST, "malformed content-length: expected exactly one value");
		}

		try {
			m_contentLength = shared::string::toNum<std::size_t>(contentLengthVec.at(0), 10);
		} catch (const std::exception& e) {
			throw http::exception(BAD_REQUEST, "invalid content-length: could not parse: " + std::string(e.what()));
		}
		if (m_contentLength > MAX_BODY_SIZE) {
			throw http::exception(PAYLOAD_TOO_LARGE, "content-length too large");
		}
	}

	void RequestParser::validateTransferEncoding() {
		const std::vector<std::string>& transferEncodingVec = m_req.getHeader("transfer-encoding");
		if (transferEncodingVec.size() != 1) {
			throw http::exception(BAD_REQUEST, "malformed transfer-encoding: expected exactly one value");
		}

		const std::string& encoding = transferEncodingVec.at(0);
		if (encoding != "chunked") {
			throw http::exception(NOT_IMPLEMENTED, "transfer-encoding " + encoding + " is not implemented");
		}
	}

	void RequestParser::reset() {
		m_buffer.reset();
		this->setState(START);
	}

	Request& RequestParser::getRequest() {
		return m_req;
	}

	bool RequestParser::isComplete() const {
		return m_state == COMPLETE;
	}

	bool RequestParser::isPending() const {
		return m_state & PENDING_MASK;
	}

	bool RequestParser::hasError() const {
		return m_state & ERROR;
	}

	shared::Buffer<REQUEST_BUFFER_SIZE>& RequestParser::getWriteBuffer() {
		return m_buffer;
	}

	bool RequestParser::isTChar(char c) const {
		return TCHAR[static_cast<unsigned char>(c)] != 0;
	}

	void RequestParser::setPendingState() {
		m_state |= PENDING_MASK;
	}

	void RequestParser::clearPendingState() {
		m_state &= ~PENDING_MASK;
	}

	void RequestParser::setState(ParseState state) {
		m_state = (m_state & PENDING_MASK) | state;
	}

} /* namespace http */