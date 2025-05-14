#!/usr/bin/python3

import os
import sys

print("Content-type: text/plain", end="\n\n")

method = os.getenv("REQUEST_METHOD")
if method == "POST":
    print(sys.stdin.read(int(os.getenv("CONTENT_LENGTH"))))
else:
    print(f"Expected POST method, got: {method}")