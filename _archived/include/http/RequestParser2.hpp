#pragma once

#include "shared/Buffer.hpp"

#include <http/Request.hpp>
#include <string>
#include <vector>

namespace http {

	enum ParseState {
		START,
		HEADERS,
		BODY,
		CHUNK_SIZE,
		CHUNK_DATA,
		COMPLETE
	};

	/**
	 * @class RequestParser
	 * @brief ...
	 */
	class RequestParser {
		public:
			RequestParser();
			~RequestParser();

			void feed(const char* data, std::size_t size);

			void reset();

			bool isComplete() const;

			const Request& getRequest() const;

		private:
			RequestParser(const RequestParser&);
			RequestParser& operator=(const RequestParser&);

			const char* readLine();
			std::string nextToken(const char*& line, char delimiter); // copying :c

			void parse();
			void parseRequestLine();
			void parseHeaders();
			void parseHeader(const char* line);
			void parseBody();
			void parseChunkSize();
			void parseChunkData();

		private:
			Request m_req; // for now (keep copying in mind)

			shared::Buffer<1024> m_buffer;
			ParseState m_state;
			std::size_t m_chunkSize;
	};

} /* namespace http */
