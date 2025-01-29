#include "http/RequestProcessor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/ErrorHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new RequestProcessor object.
	 */
	// todo figure out how to do this shit with locations
	RequestProcessor::RequestProcessor(std::vector<config::Location> locations)
		: m_res(NULL)
		, m_locations(locations) {
		m_handlers.insert(std::make_pair(GET, new GetHandler(locations.at(0))));
		m_handlers.insert(std::make_pair(POST, new PostHandler(locations.at(0))));
		m_handlers.insert(std::make_pair(DELETE, new DeleteHandler(locations.at(0)))); // todo router or idk:c
		m_handlers.insert(std::make_pair(_ERROR, new ErrorHandler(locations.at(0))));  // todo router or idk:c
	}

	/**
	 * @brief Destroys the RequestProcessor object.
	 */
	RequestProcessor::~RequestProcessor() {
		for (std::map<Method, ARequestHandler*>::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_res;
	}

	// todo check if req was valid - if not send error response
	// todo check for allowed methods
	Response* RequestProcessor::process(Request& req) {
		config::Location location;
		m_res = new Response();

		if (req.hasError()) {
			m_handlers[_ERROR]->handle(req, *m_res);
			return this->releaseResponse();
		}																									// TODO: make this not shitty
		if (req.getMethod() < _ERROR && findLocation(req.getPathData().path, m_locations, location) == 1) { // TODO: replace path with uri.path
			std::cout << "FindLocation failed; Location not found :(" << std::endl;
			req.setStatusCode(NOT_FOUND);
		}
		m_handlers[req.getMethod()]->handle(req, *m_res);
		return this->releaseResponse();
	}

	Response* RequestProcessor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
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

	int RequestProcessor::findLocation(const std::string& uri, const std::vector<config::Location>& locations, config::Location& location) {
		std::vector<std::string> file;
		std::vector<std::string> result;
		std::string path = "";
		// Split into file and path if file is present
		if (uri.length() == 0) {
			return 1;
		}
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
		config::Location tempObj;
		tempObj.locations = locations;
		const config::Location* temp = &tempObj;
		bool deeper = false;
		int len = 0;
		while (!temp->locations.empty()) {
			bool found = false;
			for (std::vector<config::Location>::const_iterator it = temp->locations.begin(); it != temp->locations.end(); ++it) {
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
		config::Location temp2 = *temp;
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
	void RequestProcessor::printLocation(const config::Location& location, int detailed) {
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
