Allowed Keywords:

base
- server (block)

in a "server" block:
- listen
- server_name
- error_page
- client_max_body_size
- location (block)

in a "location" block:
- return
- root
- autoindex
- index
- limit_exept
- location (block)
(cgi_commands, not sure which ones are needed)













Summary:

Blocks:
- server (block) - "defines the configuration for a specific virtual server. Each "server" block can contain directives that specify the server's behavior"
- location (block) - "You can use location blocks inside server to define specific paths or URIs: can be called in another location block"

Keywords:
- listen - "Specifies the IP address and port to listen to for incoming connections."
- server_name - "Defines the domain names or IP addresses that this server block should respond to."
- error_page - "Defines custom error pages for HTTP error codes."
- client_max_body_size - "Limits the size of client request bodies."
- return - "can be used to redirect requests to a different URL."
- root - "Specifies the root directory from which to serve files for this specific location."
- autoindex - "enables or disables the display of directory listings when a client requests a directory without an index file"
- index - " tells the server which file(s) to look for in a directory and serve as the default."
- limit_exept - "specify a subset of HTTP methods that are allowed, and then configure restrictions or handling for all other methods."
(cgi_commands, not sure which ones are needed)

