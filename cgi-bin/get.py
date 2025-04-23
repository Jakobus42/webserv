#!/usr/bin/env python3

body = "Hello from CGI!\r"

print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html\r")
print(f"Content-Length: {len(body)}\r")
print("\r")
print(body, end='')