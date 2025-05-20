# Webserv

A lightweight HTTP server implementation inspired by nginx, built using the reactor pattern.
Implemented using raw POSIX sockets.

## Features

### ✨ HTTP/1.1 compliant

Implemented features adhere to the HTTP/1.1 standard (according to RFC 91xx), implementing GET, POST and DELETE

### ✨ Highly extensible architecture

Modular design allows for seamless implementation of additional methods, features and configuration parameters

### ✨ Non-Blocking I/O

Performant, highly concurrent single-threaded architecture using the Reactor pattern

### ✨ Virtual-host support

Serve multiple domains from one process via per-host configs

### ✨ Support for multiple platforms

- Linux (using epoll)
- macOS and BSD-based systems (using Kqueue)

### ✨ Simple CGI/1.1 implementation

Run scripts located in cgi_dir, compliant with the CGI standard (according to RFC 3875)
Supports GET and POST

```bash
curl -X POST <host:port>/cgi-bin/scriptname.ext/param?query
```

## Building the Project

```bash
make
```

## Running Webserv

### Default Configuration

```bash
./webserv
```

This starts the server with the default configuration file at `config/default.conf`.

### Custom Configuration

```bash
./webserv <path-to-config>
```

Example:
```bash
./webserv config/all_features.conf
```

## Configuration

Webserv uses a configuration syntax inspired by nginx. The server can be configured through blocks and directives:

### HTTP Block

Global settings that apply to all servers:

```
http {
    # Global server limits (format: directive value; # default value)
    client_max_body_size 10485760;      # Max request body size (default: 10MB)
    max_header_value_length 8192;       # Max header value length (default: 8KB)
    max_header_count 128;               # Max number of headers (default: 128)
    max_header_value_count 64;          # Max header values per header (default: 64)
    max_header_name_length 256;         # Max header name length (default: 256B)
    max_uri_length 1024;                # Max URI length (default: 1024B)

    # Timeouts
    connection_timeout 60;              # Client connection timeout in seconds (default: 60s)
    cgi_timeout 60;                     # CGI script execution timeout in seconds (default: 60s)
    
    # CGI configuration
    cgi_dir ./cgi-bin/;                 # CGI scripts directory (default: ./cgi-bin/)
    cgi_interpreter .py /bin/python3;   # Map file extension to interpreter (default: none)
    cgi_interpreter .sh /bin/bash;      # Map file extension to interpreter (default: none)

    # Server blocks
    server {
        # Server configuration
    }
}
```

### Server Block

Defines a virtual server:

```
server {
    listen 127.0.0.1:8080;              # IP:port to listen on (default: 127.0.0.1:8080)
    server_name example.com alias.com;  # Server name(s) for matching Host header (default: none)
    
    root ./www/example.com;             # Document root directory (default: none, inherited)
    
    limit_except GET POST DELETE;       # Only allow these methods (default: GET, POST, DELETE)
    
    # File handling
    upload_dir uploads;                 # Upload directory for POST requests (default: none)
    autoindex on;                       # Enable directory listing if no index (default: off)
    index index.html index2.html;       # Default index files (default: none)
    
    # Custom error pages
    error_page 400 401 403 404 error_pages/40x.html;  # (default: none)
    error_page 500 501 503 504 505 508 error_pages/50x.html;  # (default: none)

    # Location blocks for path-specific settings
    location /path {
        # Location-specific configuration
    }
}
```

### Location Block

Path-specific settings:

```
location /path {
    root ./www/example.com/path;        # Document root for this location (default: inherited)
    
    limit_except GET;                   # Allow only GET requests (default: GET, POST, DELETE)
    upload_dir uploads;                 # Subdirectory to store uploads (default: none)
    autoindex off;                      # Disable directory listing (default: off)
    index index.html;                   # Default index file (default: none)
    
    # Location-specific error pages
    error_page 500 501 503 504 505 508 error_pages/50x.html;  # (default: none)
}

location /old-route {
    return 301 /new-route;              # Redirect with status code (default: none)
}
```

## Benchmark Results

Performance under heavy load (255 concurrent users, 500 requests per user):

```bash
siege -c 255 -r 500 -b 127.0.0.1:8080/
{       "transactions":                       127500,
        "availability":                       100.00,
        "elapsed_time":                        21.93,
        "data_transferred":                     0.00,
        "response_time":                        0.04,
        "transaction_rate":                  5813.95,
        "throughput":                           0.00,
        "concurrency":                        221.81,
        "successful_transactions":            127500,
        "failed_transactions":                     0,
        "longest_transaction":                  3.14,
        "shortest_transaction":                 0.00
}
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
