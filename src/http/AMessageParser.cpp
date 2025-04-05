#include "http/AMessageParser.hpp"

#include "http/AMessage.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

namespace http {

	MessageParserConfig::MessageParserConfig()
		: maxBodySize(10 * 1024 * 1024)	 // 10MB
		, maxHeaderValueLength(8 * 1024) // 8KB
		, maxHeaderCount(128)			 // 128 headers
		, maxHeaderValueCount(64)		 // 64 values
		, maxHeaderNameLength(256)		 // 256B
	{}

	shared::string::StringView AMessageParser::CRLF = shared::string::StringView("\r\n");
	const char AMessageParser::TCHAR[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '!', 0, '#', '$', '%', '&', '\'', 0, 0, '*', '+', 0, '-', '.', 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0, 0, 0, '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, '|', 0, '~', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	shared::string::StringView AMessageParser::WHITESPACE = shared::string::StringView(" \t");

	AMessageParser::AMessageParser(const MessageParserConfig& conf)
		: m_baseConfig(conf)
		, m_message(NULL)
		, m_state(START_LINE)
		, m_buffer()
		, m_contentLength(0) {}

	AMessageParser::~AMessageParser() { delete m_message; }

	bool AMessageParser::parse() {
		if (!m_message) {
			m_message = createMessage();
		}

		while (m_state != COMPLETE) {
			ParseResult result = NEED_DATA;

			switch (m_state) {
				case START_LINE:
					result = parseStartLine();
					if (result == DONE) m_state = HEADERS;
					break;

				case HEADERS:
					result = parseHeaderLine();
					if (result == DONE) m_state = isChunked() ? CHUNK_SIZE : BODY;
					break;

				case TRAILING_HEADERS:
					result = parseHeaderLine();
					if (result == DONE) m_state = COMPLETE;
					break;

				case CHUNK_SIZE:
					result = parseChunkSize();
					if (result == DONE) m_state = (m_contentLength == 0) ? TRAILING_HEADERS : CHUNK_BODY;
					break;

				case BODY:
					result = parseBody(false);
					if (result == DONE) m_state = COMPLETE;
					break;

				case CHUNK_BODY:
					result = parseBody(true);
					if (result == DONE) m_state = CHUNK_SIZE;
					break;

				case COMPLETE:
					break;
			}

			if (result == NEED_DATA) {
				break;
			}
		}

		return m_state == COMPLETE;
	}

	shared::container::Buffer<AMessageParser::BUFFER_SIZE>& AMessageParser::getReadBuffer() { return m_buffer; }

	void AMessageParser::reset() {
		m_state = START_LINE;
		delete m_message;
		m_message = NULL;
		m_contentLength = 0;
	}

	/* Message management */

	AMessage* AMessageParser::releaseMessage() {
		AMessage* released = m_message;
		m_message = NULL;
		return released;
	}

	/* Shared */

	std::pair<shared::string::StringView /*line */, bool /*ok*/> AMessageParser::readLine() {
		const char* lineStart = m_buffer.readPtr();
		const char* lineEnd = m_buffer.find(CRLF.data());
		if (lineEnd == NULL) {
			return std::make_pair(shared::string::StringView(), false);
		}

		m_buffer.consume((lineEnd - lineStart) + CRLF.size());
		return std::make_pair(shared::string::StringView(lineStart, lineEnd - lineStart), true);
	}

	bool AMessageParser::isChunked() const {
		return m_message->hasHeader("transfer-encoding") &&
			m_message->getHeader("transfer-encoding").front() == "chunked";
	}

	bool AMessageParser::isTChar(char c) const { return TCHAR[static_cast<unsigned char>(c)] != 0; }

	bool AMessageParser::isVChar(char c) const { return c >= '!' && c <= '~'; }

	/* Parsers */

	/*
	   message-header = field-name ":" [ field-value ]
	   field-name     = token
	   field-value    = *( field-content | LWS )
	   field-content  = <the OCTETs making up the field-value
						and consisting of either *TEXT or combinations
						of token, separators, and quoted-string>
	*/
	AMessageParser::ParseResult AMessageParser::parseHeaderLine() {
		std::pair<shared::string::StringView /*line */, bool /*ok*/> ret = readLine();
		if (ret.second == false) {
			return NEED_DATA;
		}

		shared::string::StringView line = ret.first;
		if (line.empty()) {
			validateHeaders();
			return DONE;
		}

		if (m_message->getHeaders().size() > m_baseConfig.maxHeaderCount - 1) {
			throw HttpException(PAYLOAD_TOO_LARGE, "header amount exceeds limit");
		}

		try {
			shared::string::StringView keyView = extractHeaderKey(line);
			line.remove_prefix(keyView.size() + 1 /*colon len*/);
			std::vector<shared::string::StringView> valueViews = extractHeaderValues(line);
			m_message->setHeader(keyView, valueViews);
			if (m_message->getHeaders().size() > m_baseConfig.maxHeaderValueCount - 1) {
				throw HttpException(PAYLOAD_TOO_LARGE, "field-value amount exceeds limit");
			}
		} catch (const HttpException& e) {
			throw HttpException(e.getStatusCode(), "invalid header: " + e.getMessage());
		}
		return CONTINUE;
	}

	void AMessageParser::validateHeaders() {
		bool hasTransferEncoding = m_message->hasHeader("transfer-encoding");
		bool hasContentLength = m_message->hasHeader("content-length");

		// RFC 7230 section 3.3.3
		if (hasTransferEncoding && hasContentLength) {
			LOG_WARNING("suspicious HTTP message: Transfer-Encoding \
				and Content-Length are present - Content-Length will be ignored.");
		}

		if (hasContentLength && !isChunked()) {
			const std::vector<std::string>& values = m_message->getHeader("content-length");

			// RFC 7230 section 3.3.2
			if (values.size() > 1) {
				for (std::size_t i = 1; i < values.size(); ++i) {
					if (values[i] != values[0]) {
						throw HttpException(BAD_REQUEST, "conflicting content-length headers");
					}
				}
			}

			try {
				m_contentLength = shared::string::toNum<std::size_t>(values.front());
			} catch (const std::exception& e) {
				throw HttpException(BAD_REQUEST, "invalid content-length: could not parse: " + std::string(e.what()));
			}
			if (m_contentLength > m_baseConfig.maxBodySize) {
				throw HttpException(PAYLOAD_TOO_LARGE, "content-length exceeds size limit");
			}
		}
	}

	shared::string::StringView AMessageParser::extractHeaderKey(const shared::string::StringView& line) const {
		std::size_t colonPos = line.find(':');
		if (colonPos == shared::string::StringView::npos) {
			throw HttpException(BAD_REQUEST, "missing ':' after field-name");
		}
		if (colonPos == 0) {
			throw HttpException(BAD_REQUEST, "empty field-name");
		}
		if (colonPos > m_baseConfig.maxHeaderNameLength) {
			throw HttpException(PAYLOAD_TOO_LARGE, "field-name exceeds size limit");
		}

		for (std::size_t i = 0; i < colonPos; ++i) {
			if (!isTChar(line[i])) {
				throw HttpException(BAD_REQUEST, std::string("invalid character in field-name '") + line[i] + '\'');
			}
		}
		return line.substr(0, colonPos);
	}

	std::vector<shared::string::StringView> AMessageParser::extractHeaderValues(const shared::string::StringView& line) const {
		std::vector<shared::string::StringView> values;
		std::size_t start = 0;

		values.reserve(8);
		while (start < line.size()) {
			std::size_t end = line.find(',', start);
			if (end == shared::string::StringView::npos) {
				end = line.size();
			}
			shared::string::StringView rawValue(line.data() + start, end - start);

			shared::string::StringView value = rawValue.substr(
				rawValue.find_first_not_of(WHITESPACE),
				rawValue.find_last_not_of(WHITESPACE));

			for (std::size_t i = 0; i < value.size(); ++i) {
				if (!isVChar(value[i]) && !std::strchr(WHITESPACE.data(), value[i])) {
					throw HttpException(BAD_REQUEST, std::string("invalid character in field-value '") + line[i] + '\'');
				}
			}

			if (value.size() > m_baseConfig.maxHeaderValueLength) {
				throw HttpException(PAYLOAD_TOO_LARGE, "field-value exceeds size limit");
			}
			values.push_back(value);

			start = end + 1;
		}
		return values;
	}

	AMessageParser::ParseResult AMessageParser::parseChunkSize() {
		std::pair<shared::string::StringView /*line */, bool /*ok*/> ret = readLine();
		if (ret.second == false) {
			return NEED_DATA;
		}
		shared::string::StringView line = ret.first;

		// RFC 7230 section 4.1.1 (ignore extension)
		std::size_t semicolonPos = line.find(';');
		shared::string::StringView sizeView = line.substr(0, semicolonPos);

		try {
			m_contentLength = shared::string::toNum<std::size_t>(sizeView.toString(), std::hex);
		} catch (std::exception& e) {
			throw HttpException(BAD_REQUEST, "invalid chunk size: " + std::string(e.what()));
		}
		return DONE;
	}

	// this function flow is kinda fucked up but idc (maybe I do) .-.
	AMessageParser::ParseResult AMessageParser::parseBody(bool isChunked) {
		if (m_contentLength > 0) {
			std::size_t available = std::min(m_buffer.size(), m_contentLength);
			m_message->appendBody(shared::string::StringView(m_buffer.readPtr(), available));
			if (m_message->getBody().size() > m_baseConfig.maxBodySize) {
				throw HttpException(PAYLOAD_TOO_LARGE, "body exceeds size limit");
			}

			m_buffer.consume(available);
			m_contentLength -= available;
			if (m_contentLength > 0) {
				return NEED_DATA;
			}
			return isChunked ? CONTINUE : DONE;
		} else if (isChunked) { // consume CRLF after last chunk
			std::pair<shared::string::StringView /*line */, bool /*ok*/> ret = readLine();
			if (ret.second == false) {
				return NEED_DATA;
			}
		}
		return DONE;
	}

} /* namespace http */