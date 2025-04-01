#include "http2/RequestParser.hpp"
#include "http2/ResponseParser.hpp"

#include <cstring>

int main() {
	http2::MessageParserConfig conf;
	http2::RequestParser parser(conf);
	const char* httpRequest =
		"Host: www.example.com\r\n"
		"Content-Length: 27\r\n"
		"\r\n"
		"username=john&password=pass";

	shared::Buffer<http2::RequestParser::BUFFER_SIZE>& buffer = parser.getReadBuffer();
	std::size_t size = std::strlen(httpRequest);
	buffer.prepareWrite(size);
	std::memcpy(buffer.writePtr(), httpRequest, size);
	buffer.advanceWriter(size);

	parser.parse();
	http2::Request* req = parser.releaseRequest();
	std::cout << req->toString() << std::endl;
}
