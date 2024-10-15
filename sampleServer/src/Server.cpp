#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>

int PORT = 9090;

static int createSocket() {
  int serverSock = socket(AF_INET, SOCK_STREAM, 0); // AF_INET for IPv4 ; SOCK_STREAM for TCP
  if (serverSock == -1) {
    throw std::runtime_error("Socket creation failed");
  }
  return serverSock;
}

static void bindSocket(int serverSock) {
  sockaddr_in serverAddr{};

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY; //takes the default local host address
  serverAddr.sin_port = htons(PORT); //converts to network bytes

  if (bind(serverSock, reinterpret_cast<sockaddr *>(&serverAddr),
           sizeof(serverAddr)) < 0) {
    throw std::runtime_error("Bind failed");
  }
}

static void startListening(int serverSock) {
  if (listen(serverSock, 3) < 0) {
    throw std::runtime_error("Listen failed");
  }
}

static int acceptClient(int serverSock) {
  sockaddr_in clientAddr{};
  socklen_t addrLen = sizeof(clientAddr);
  int clientSock =
      accept(serverSock, reinterpret_cast<sockaddr *>(&clientAddr), &addrLen); //This is blocking ; there is a flag to activate NON BLOCKING
  if (clientSock < 0) {
    throw std::runtime_error("Accept failed");
  }
  return clientSock;
}

static std::string readRequest(int clientSock) {
  char buffer[1024] = {0};
  ssize_t bytesRead = recv(clientSock, buffer, sizeof(buffer), 0);
  if (bytesRead < 0)
    throw std::runtime_error("Read failed");
  return std::string(buffer, bytesRead);
}

static void sendResponse(int clientSock, const std::string &content) {
  std::string response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: " +
                         std::to_string(content.length()) +
                         "\r\n"
                         "\r\n" +
                         content;
  ssize_t bytesSent = send(clientSock, response.c_str(), response.length(), 0);
  if (bytesSent < 0) {
    throw std::runtime_error("Send failed");
  }
}

static void handleClient(int clientSock) {
  std::string request = readRequest(clientSock);
  std::cout << "Received request:\n" << request << std::endl;
  sendResponse(clientSock, "Hello, world!");
  close(clientSock);
}

int main() {
  try {
    int serverSock = createSocket();
    bindSocket(serverSock);
    startListening(serverSock);
    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
      try {
        int clientSock = acceptClient(serverSock);
        handleClient(clientSock);
      } catch (const std::exception &e) {
        std::cerr << "Client error: " << e.what() << std::endl;
      }
    }
    close(serverSock);
  } catch (const std::exception &e) {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }
}