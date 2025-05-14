#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/plain\n")
post_data = sys.stdin.read(int(os.environ.get("CONTENT_LENGTH")))
print(post_data)