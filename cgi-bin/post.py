#!/usr/bin/env python3
import sys
import os

content_length = os.environ.get('CONTENT_LENGTH')
input_data = ''

if content_length:
    content_length = int(content_length)
    if content_length > 0:
        input_data = sys.stdin.read(content_length)

body = f"thank you for this beautiful string u sent me: {input_data}"

print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html\r")
print(f"Content-Length: {len(body)}\r")
print("\r")
print(body, end='')