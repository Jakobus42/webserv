# Config File Basic Syntax

## Table of Contents
1. [Special_Characters](#special-characters)
2. [Allowed_Keywords](#allowed-keywords)
3. [Keyword_Summary](#keyword-summary)

## Special Characters
- **#** Identifies that everything until the next newline is a comment and not interpreted.
- **{}** Identifies the start / end of a block directly after **server / location** Keyword. Has to be on otherwise empty line.
- **;** Identifies the end of a keyword line, if keyword doesn't expect block after.


## Allowed Keywords

### base
- **server (block)**		"server {}"

### in a "server" block:
- **listen**				"listen <0-255.0-255.0-255:0-65535>" / "listen <0-65535>"
- **server_name**			"server_name <name> <name> ..."
- **error_page**			"error_page <100-599> <100-599> ... <html-file>"
- **client_max_body_size**	"client_max_body_size <0-2147483647>"
- **location (block)**		"location <name> {}"

### in a "location" block:
- **limit_exept**			"limit_exept <HTTP-Method> <HTTP-Method> ..."
- **return**				"return <300-399> <url>"
- **root**					"root <path>"
- **autoindex**				"autoindex <on/off>"
- **index**					"index <html-file> <html-file> ..."
- **location (block)**		"location <name> {}"
//TODO (cgi_commands, not sure which ones are needed)


## Keyword Summary
### Blocks:
- **server (block)** 		- "defines the configuration for a specific virtual server. Each keyword can specify behavior"
- **location (block)** 		- "You can use location blocks to define specific handeling for specific paths or URIs"

### Keywords:
- **listen** 				- "Specifies the IP address and port to listen to for incoming connections."
- **server_name**			- "Defines the domain names or IP addresses that this server block should respond to."
- **error_page**			- "Defines custom error pages for HTTP error codes."
- **client_max_body_size** 	- "Limits the size of client request bodies."
- **return**				- "can be used to redirect requests to a different URL."
- **root** 					- "Specifies the root directory from which to serve files for this specific location."
- **autoindex** 			- "enables the display of directory listings when a client requests a directory without an index file"
- **index** 				- " tells the server which file(s) to look for in a directory and serve as the default."
- **limit_exept** 			- "specify HTTP methods that are allowed, forbid the others"
(cgi_commands, not sure which ones are needed)

