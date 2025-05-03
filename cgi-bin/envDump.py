#!/usr/bin/python3
import os

body = "CGI env vars:\n"
for key, value in sorted(os.environ.items()):
    body += f"{key}: {value}\n"

content_length = len(body)

print(f"Content-Length: {content_length}")
print("Content-Type: text/plain")
print("")
print(body, end="")