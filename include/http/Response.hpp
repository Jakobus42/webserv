#pragma once

#include "http/http.hpp"
#include "shared/Buffer.hpp"
#include "shared/NonCopyable.hpp"

#include <map>
#include <vector>

#define RESPONSE_BUFFER_SIZE 16 * 1024

namespace http {

	/**
	 * @class Response
	 * @brief ...
	 */
	class Response : shared::NonCopyable {
		public:
			Response();
			~Response();

			void serialize();
			inline std::string getDateString();

			void setStatusCode(StatusCode statusCode);
			void setBody(const std::string& body);
			void setHeader(const std::string& key, const std::string& value);
			void appendToBody(const char* data, std::size_t len);

			std::string& getData();
			StatusCode getStatusCode() const;
			const std::string& getBody() const;
			std::size_t getReadPos() const;

			void advanceReadPos(std::size_t n);
			bool eof() const;

		private:
			StatusCode m_statusCode;
			std::map<std::string, std::vector<std::string> > m_headers;
			std::string m_body;
			std::size_t m_readPos;

			std::string m_data;
	};

} /* namespace http */
