#pragma once

#include "config/Parser.hpp"
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

			void setStatusCode(StatusCode statusCode);
			void setBody(const std::string& body);
			void setHeader(const std::string& key, const std::string& value);

			shared::Buffer<RESPONSE_BUFFER_SIZE>& getData();

		private:
			StatusCode m_statusCode;
			std::map<std::string, std::vector<std::string> > m_headers;
			std::string m_body;

			shared::Buffer<RESPONSE_BUFFER_SIZE> m_data;
	};

} /* namespace http */
