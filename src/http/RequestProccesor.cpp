#include "http/RequestProccesor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new RequestProccesor object.
	 */
	// todo figure out how to do this shit with locations
	RequestProccesor::RequestProccesor(std::vector<config::t_location> locations)
		: m_res(NULL)
		, m_locations(locations) {
		m_handlers.insert(std::make_pair(GET, new GetHandler(locations.at(0))));
		m_handlers.insert(std::make_pair(POST, new PostHandler(locations.at(0))));
		m_handlers.insert(std::make_pair(DELETE, new DeleteHandler(locations.at(0)))); // todo router or idk:c
	}

	/**
	 * @brief Destroys the RequestProccesor object.
	 */
	RequestProccesor::~RequestProccesor() {
		for (std::map<Method, ARequestHandler*>::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_res;
	}

	// todo check if req was valid - if not send error response
	// todo check for allowed methods
	Response* RequestProccesor::process(Request& req) {
		m_res->setStatusCode(OK);
		if (!checkRequestData(req)) {
			// Create error page
		}
		config::t_location location;
		if (findLocation(req.getPathData().pure_path, m_locations, location) == 1) {
			m_res->setStatusCode(NOT_FOUND);
		}
		m_res = new Response();
		m_handlers[req.getMethod()]->handle(req, *m_res);
		m_res->setBody("Hello, World!");
		return this->releaseResponse();
	}

	Response* RequestProccesor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
	}

	/**
	 * @brief Parses Absolute path and saves only the pure path in "m_truePath" src: (RFC 9112 3.2.2)
	 * @details
	 * absolute-URI = scheme ":" hier-part [ "?" query ]
	 * @param path
	 */
	bool RequestProccesor::parseAbsoluteForm(const std::string& path, Request& request) {
		std::string scheme;
		std::string query = "";
		std::string authority;
		std::string pure_path;
		if (path.find(':') == std::string::npos) {
			return false;
		}
		scheme = path.substr(0, path.find(":")); // scheme before the first colon
		if (scheme == "" || std::isalpha(scheme[0]) == 0 || scheme.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-.") != std::string::npos) {
			return false;
		}
		if (path.find('?') != std::string::npos) { // optional query part after the first question mark
			query = path.substr(path.find('?') + 1);
		}
		pure_path = path.substr(path.find(':') + 1, path.find('?') - path.find(':') - 1);
		if (pure_path[0] != '/' && pure_path[1] != '/') { // authority part starts with two slashes
			return false;
		}
		pure_path = pure_path.substr(2);
		if (pure_path.find('/') == std::string::npos) {
			return false;
		}
		authority = pure_path.substr(0, pure_path.find('/'));
		if (authority.empty()) {
			return false;
		}
		pure_path = pure_path.substr(pure_path.find('/'));
		http::PathData& pathData = request.getPathData();
		pathData.query = query;
		pathData.pure_path = pure_path;
		pathData.scheme = scheme;
		pathData.authority = authority;
		return true;
	}

	bool RequestProccesor::parseOriginForm(const std::string& path, Request& request) {
		std::string authority;
		std::string pure_path;
		std::string query;
		if (path.find('?') != std::string::npos) { // optional query part after the first question mark
			query = path.substr(path.find('?') + 1);
		}
		pure_path = path.substr(0, path.find('?'));
		if (pure_path[0] != '/') {
			return false;
		}
		int found = 0;
		for (std::map<std::string, std::vector<std::string> >::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it) {
			if (it->first == "host") {
				if (found == 1) {
					return false;
				}
				if (it->second.size() != 1) {
					return false;
				}
				authority = it->second[0];
				found = 1;
			}
		}
		if (found == 0) {
			return false;
		}
		http::PathData& pathData = request.getPathData();
		pathData.query = query;
		pathData.pure_path = pure_path;
		pathData.authority = authority;
		pathData.scheme = "";
		return true;
	}

	/**
	 * @brief find request target form, check it and reconstruct it (RFC 9112 3.2)
	 * @details has to be either "Origin-form" or "Absolute-form" (others only with unsupported methods)
	 * @param request
	 */
	bool RequestProccesor::checkAndReconstructTargetUri(Request& request) {
		if (request.getUriRaw()[0] == '/') {
			if (!parseOriginForm(request.getUriRaw(), request)) {
				m_res->setStatusCode(BAD_REQUEST);
				return false;
			}

		} else {
			if (!parseAbsoluteForm(request.getUriRaw(), request)) {
				m_res->setStatusCode(BAD_REQUEST);
				return false;
			}
		}
		return true;
	}

	/**
	 * @brief Check if the request data is valid, and set the status code accordingly
	 * @details Stuff being checked:
	 * - Method is GET, POST or DELETE
	 * - Uri is valid, in the correct form and gets reconstructed
	 * - Version is HTTP/1.1
	 * @param request The request to check
	 */
	bool RequestProccesor::checkRequestData(Request& request) {
		if (request.getMethod() != GET && request.getMethod() != POST && request.getMethod() != DELETE) {
			m_res->setStatusCode(NOT_IMPLEMENTED);
			return false;
		}
		if (request.getUriRaw().length() > 8192) {
			m_res->setStatusCode(URI_TOO_LONG);
			return false;
		}
		if (!checkAndReconstructTargetUri(request)) {
			return false;
		}
		if (request.getVersion() != "HTTP/1.1") {
			m_res->setStatusCode(HTTP_VERSION_NOT_SUPPORTED);
			return false;
		}
		// (RFC 9112 3.2)
		/* 		if (request.getRequestData().headers.find("Host") == request.getRequestData().headers.end()
					|| request.getRequestData().headers.find("Host")->second.size() != 1) {
					m_res->setStatusCode(BAD_REQUEST);
					m_type = ERROR;
					return;
				} */
		return true;
	}

	int RequestProccesor::testParseURI(const std::string& uri, int mode) {
		std::cout << "-----------------------------------" << std::endl;
		Request req;
		if (mode == 0) {
			std::vector<std::string> host;
			host.push_back("localhost");
			req.setHeader("Host", 4, "Localhost", 9);
			if (parseOriginForm(uri, req)) {
				std::cout << "Origin form parsed successfully" << std::endl;
			} else {
				std::cout << "Origin form failed" << std::endl;
				std::cout << "-----------------------------------" << std::endl;
				return 1;
			}
		} else {
			if (parseAbsoluteForm(uri, req)) {
				std::cout << "Absolute form parsed successfully" << std::endl;
			} else {
				std::cout << "Absolute form failed" << std::endl;
				std::cout << "-----------------------------------" << std::endl;
				return 1;
			}
		}
		const http::PathData& pathData = req.getPathData();
		std::cout << "Scheme: " << pathData.scheme << std::endl;
		std::cout << "Authority: " << pathData.authority << std::endl;
		std::cout << "Pure path: " << pathData.pure_path << std::endl;
		std::cout << "Query: " << pathData.query << std::endl;
		std::cout << "-----------------------------------" << std::endl;
		return 0;
	}

	int comparePaths(const std::vector<std::string>& LocationPath, const std::vector<std::string>& uriPath, int& len) {
		unsigned long paths = 0;
		if (LocationPath.size() > uriPath.size()) {
			return 1;
		}
		if (LocationPath.size() == 0 && uriPath.size() != 0) {
			return 1;
		}
		for (std::vector<std::string>::const_iterator it = LocationPath.begin(); it != LocationPath.end(); ++it) {
			if (*it != uriPath[paths]) {
				return 1;
			}
			paths++;
		}
		len = paths;
		return 0;
	}

	int RequestProccesor::findLocation(const std::string& uri, const std::vector<config::t_location>& locations, config::t_location& location) {
		std::vector<std::string> file;
		std::vector<std::string> result;
		std::string path = "";
		// Split into file and path if file is present
		if (uri[uri.size() - 1] != '/') {
			int last = uri.find_last_of('/');
			file.push_back(uri.substr(last + 1));
			path = uri.substr(0, last + 1);
		} else {
			path = uri;
		}
		// Split path into vector
		if (shared::string::splitPath(path, result) == 1) {
			return 1;
		}
		// find the location
		config::t_location tempObj;
		tempObj.locations = locations;
		const config::t_location* temp = &tempObj;
		bool deeper = false;
		int len = 0;
		while (!temp->locations.empty()) {
			bool found = false;
			for (std::vector<config::t_location>::const_iterator it = temp->locations.begin(); it != temp->locations.end(); ++it) {
				if (comparePaths(it->path, result, len) == 0) {
					temp = &(*it);
					found = true;
					break;
				}
			}
			if (!found) {
				if (!deeper) {
					return 1;
				} else {
					break;
				}
			}
			deeper = true;
		}
		config::t_location temp2 = *temp;
		// Set the file and root
		if (!file.empty()) {
			temp2.index = file;
		}
		if (temp2.root != "") {
			while (len > 0) {
				result.erase(result.begin());
				len--;
			}
			if (temp2.root[temp2.root.size() - 1] != '/') {
				temp2.root += "/";
			}
		}
		for (std::vector<std::string>::const_iterator it = result.begin(); it != result.end(); ++it) {
			temp2.root += *it + "/";
		}
		location = temp2;
		return 0;
	}

	/**
	 * @brief prints a location.
	 * @param locations the locations to print.
	 * @param detailed toggles between detailed and simple output. (0 for simple, 1
	 * for detailed)
	 */
	void RequestProccesor::printLocation(const config::t_location& location, int detailed) {
		std::cout << "--------------------------" << std::endl;
		std::cout << "Location: ";
		for (std::vector<std::string>::const_iterator it = location.path.begin();
			 it != location.path.end();
			 ++it) {
			std::cout << *it << "/";
		}
		std::cout << std::endl;
		if (detailed) {
			if (location.root != "")
				std::cout << "Root: " << location.root << std::endl;
			if (location.autoindex)
				std::cout << "Autoindex: on" << std::endl;
			else
				std::cout << "Autoindex: off" << std::endl;
			if (location.methods.size() > 0) {
				std::cout << "Methods: ";
				for (unsigned long k = 0; k < location.methods.size();
					 k++) {
					std::cout << location.methods[k] << " ";
				}
				std::cout << std::endl;
			}
			if (location.index.size() > 0) {
				std::cout << "Index: ";
				for (unsigned long k = 0; k < location.index.size(); k++) {
					std::cout << location.index[k] << " ";
				}
				std::cout << std::endl;
			}
		}
		std::cout << "--------------------------" << std::endl;
	}

} /* namespace http */
