#pragma once

#include "core/CGIProcessor.hpp"
#include "http/ResponseParser.hpp"
#include "io/IEventHandler.hpp"

namespace core {

	class CGIEventHandler : public io::IEventHandler {
		public:
			// sets pointer to CGI response
			explicit CGIEventHandler(CGIProcessor& processor, const http::Request& request, http::Response*& response);
			~CGIEventHandler();

			io::EventResult onReadable(int32_t fd);

			io::EventResult onWriteable(int32_t fd);

			io::EventResult onError(int32_t fd);

		private:
			CGIProcessor& m_processor;
			const http::Request& m_request;
			http::Response*& m_response;

			std::size_t m_bytesWritten;
			http::ResponseParser m_responseParser;
	};

} /* namespace core */
