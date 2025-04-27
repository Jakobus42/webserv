#!/usr/bin/python3
import os

body = "CGI env vars:\r\n"
for key, value in sorted(os.environ.items()):
    body += f"{key}: {value}\r\n"

content_length = len(body)

print("HTTP/1.1 200 OK\r")
print(f"Content-Length: {content_length}\r")
print("Content-Type: text/plain\r")
print("\r")
print(body, end="")