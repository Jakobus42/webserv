#!/usr/bin/env python3

body = "Hello from CGI!"

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print(f"Content-Length: {len(body)}")
print("")
print(body, end='')