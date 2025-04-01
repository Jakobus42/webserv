#include "http2/RequestParser.hpp"
#include "http2/ResponseParser.hpp"

#include <cstring>

int main() {
	http2::MessageParserConfig conf;
	http2::RequestParser parser(conf);
	const char* httpRequest =
		"GET /index.html?param=value HTTP/1.1\r\n"
		"Host: www.example.com\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml\r\n"
		"Accept-Language: en-US,en;q=0.9\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n"
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
