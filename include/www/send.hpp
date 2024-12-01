#pragma once

#define RN "\r\n"

int createSocket(int port);
bool has_transmission_end(std::vector<char>& data);
void sendGarbage(int clientSocketFd);
int doKoolShit(const int port);
