#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "../../include/www/Client.hpp"
#include "../../include/www/Socket.hpp"

#define DEFAULT_CHUNK_SIZE 64
const char* SEQUENCE = "\r\n\r\n";

typedef std::map<std::string, std::string> t_fields;
typedef std::vector<char> t_bytes;

// int createSocket(int port) {
// 	struct sockaddr_in sin;
// 	int fd;
// 	int opt = 1;

// 	fd = socket(AF_INET, SOCK_STREAM, 0);

// 	if (fd == -1) {
// 		return -1;
// 	}
// 	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
// 		close(fd);
// 		return -1;
// 	}
// 	sin.sin_family = AF_INET;
// 	sin.sin_addr.s_addr = htonl(0x7F000001);  // 127.0.0.1
// 	sin.sin_port = htons(port);
// 	if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0 ||
// 		listen(fd, 10) < 0) {
// 		close(fd);
// 		return -1;	// might wanna return proper exit codes (negative) instead
// 	}
// 	return fd;
// }

bool hasTwoConsecutiveLineBreaks(std::string& received) {
  const char END_SEQUENCE[4] = {'\r', '\n', '\r', '\n'};
  if (std::search(received.begin(), received.end(), END_SEQUENCE, END_SEQUENCE + 4) != received.end()) {
    return true;
  }
  return false;
  // if doesnt contain \r\n\r\n, continue on (return false)
  // if contains /r/n/r/n, make sure to cut that off and possibly save for the
  // next request find out and set headers as you read
}

bool has_transmission_end(t_bytes& data) {
  if (data.size() < sizeof(SEQUENCE)) {
    return false;
  }
  t_bytes::iterator it = data.begin();
  for (int i = 0; i < 4; i++) {
    if (it[i] != SEQUENCE[i]) return false;
  }
  return true;
}

void sendGarbage(int clientSocketFd) {
  char sendBuffer[2048] =
      "HTTP/1.1 300 CockAndBalls\r\n"
      "Date: Sun, 18 Oct 2012 10:36:20 GMT\r\n"
      "Server: Apache/2.2.14 (Win32)\r\n"
      "Content-Length: 137\r\n"
      "Connection: Closed\r\n"
      "Content-Type: text/html; charset=UTF-8;\r\n"
      "\r\n"
      "<!DOCTYPE html><html><head><title>Basic "
      "Page</title></head><body><h1>Hello, World!</h1><p>This is a simple "
      "HTML page.</p></body></html>\r\n";

  std::cout << "now sending" << std::endl;
  send(clientSocketFd, sendBuffer, sizeof(sendBuffer) - 1, 0);
  std::cout << "sent" << std::endl;
  bzero(&sendBuffer, sizeof(sendBuffer));
}

t_bytes::iterator getCurrentTokenEnd(t_bytes::iterator begin, t_bytes::iterator end) {
  const char sequence[2] = {'\r', '\n'};
  t_bytes::iterator foo = std::search(begin, end, sequence, sequence + 2);
  if (foo == end) return end;
  return foo;
}

t_fields parseHeaders(std::string& receivedBytes) {
  t_fields newHeaders;
  size_t seqEndIndex = receivedBytes.find(SEQUENCE, 0UL, sizeof(SEQUENCE));
  if (seqEndIndex == std::string::npos) {
    return newHeaders;  // don't yet have end of transmission
  }

  // find start of token
  // should be right there

  // read until "\r\n"

  // expect chunk to have header format

  // if encountering "\r\n" at the start of a line, stop parsing head ->
  // return expect body only if certain conditions are met

  // if expecting body, write body into second thingy
  // starting right after double newline
  std::istringstream stream(receivedBytes);
  std::string line;

  // Read each line until we find an empty line (which indicates the end of
  // headers)
  while (std::getline(stream, line) && line != "\r") {
    size_t delimiterPos = line.find(": ");
    if (delimiterPos != std::string::npos) {
      std::string headerName = line.substr(0, delimiterPos);
      std::string headerValue = line.substr(delimiterPos + 2);
      newHeaders[headerName] = headerValue;
      // if a header key already exists, concatenate as a list
      // with some exceptions (I think e.g. content-size can't be duplicate)
      // also, parse out some numbers where relevant
    }
  }

  return newHeaders;
}

std::pair<www::Socket, www::Client> getClientSocketPair(int port) {
  www::Socket socket(port);
  www::Client client;

  if (socket.init() == false) throw std::exception();
  if (client.accept(socket.getFd()) == false) {
    socket.close();
    throw std::exception();
  }
  return std::make_pair(socket, client);
}

int doKoolShit(const int port) {
  std::vector<www::Socket> sockets(0);
  std::vector<www::Client> clients(0);
  t_fields currentHeaders;
  // okay, strictly it's called field
  // there's header fields and apparently trailer fields as well
  // fields sounds better than header_list methinks and it's not a list xd

  try {
    std::pair<www::Socket, www::Client> pair = getClientSocketPair(port);
    sockets.push_back(pair.first);
    clients.push_back(pair.second);
  } catch (std::exception& e) {
    std::cout << "Oh crap!" << std::endl;
    return 1;
  }

  char buffer[DEFAULT_CHUNK_SIZE];
  std::string received;

  while (true) {
    int bytes_read = 0;
    bzero(&buffer, sizeof(buffer));
    // if receiving Content-Length and it's nonzero, first keep reading
    // until \r\n\r\n then keep reading exactly until content_length is
    // extracted keep in mind we might receive d abcd abcd abc\r \n\r\na
    // bcda bcd in this case if Content-Length is 8, we should only read 7
    // more bytes since the previous read already contains one of the eight
    // body bytes
    bytes_read = recv(clients.at(0).getFd(), &buffer, DEFAULT_CHUNK_SIZE, 0);
    std::cout << "Received " << bytes_read << std::endl;
    if (bytes_read == -1) {
      std::cout << "Ah fuck" << std::endl;
      return 1;
    }
    if (bytes_read == 0) {
      std::cout << "Huh, received 0" << std::endl;
      break;
    }
    received.reserve(received.size() + DEFAULT_CHUNK_SIZE);
    received.insert(received.end(), buffer, buffer + DEFAULT_CHUNK_SIZE);
    // parse protocol version
    // parse path
    // parse method
    // then come the headers
    // parse it and set variables
    // at the same time figure out if there's
    // two consecutive newlines
    currentHeaders = parseHeaders(received);
    if (hasTwoConsecutiveLineBreaks(received)) break;
    // blocks forever if total length % 4 == 0
  }
  std::cout << "Headers received:" << std::endl;
  for (t_fields::iterator it = currentHeaders.begin(); it != currentHeaders.end(); ++it) {
    std::cout << "- " << it->first << ": " << it->second << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Full received request:" << std::endl;
  for (size_t i = 0; i < received.size(); i++) {
    std::cout << received[i];
  }
  std::cout << std::endl;

  std::cout << "length received: " << received.size() << std::endl;

  for (std::vector<www::Socket>::iterator it = sockets.begin(); it != sockets.end(); ++it) {
    it->close();
  }
  sendGarbage(clients.at(0).getFd());
  close(clients.at(0).getFd());
  return 0;
}
