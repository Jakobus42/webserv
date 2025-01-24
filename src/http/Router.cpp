#include "http/Router.hpp"
#include "http/Response.hpp"

namespace http {

/**
 * @brief Constructs a new Router object.
 */
Router::Router() {

}

/**
 * @brief Destroys the Router object.
 */
Router::~Router() {

}

/**
 * @brief Copy constructor.
 * @param other The other Router object to copy.
 */
Router::Router(const Router &other) {
	(void)other;
}

/**
 * @brief Copy assignment operator.
 * @param other The other Router object to assign from.
 * @return A reference to the assigned Router object.
 */
Router& Router::operator=(const Router &rhs) {
    (void)rhs;
	return *this;
}

int comparePaths(const std::vector<std::string>& LocationPath, const std::vector<std::string>& uriPath, int &len) {
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

int Router::findLocation(const std::string& uri, std::vector<config::t_location>& locations, config::t_location& location, http::ResponseType& m_type) {
	std::vector<std::string> file;
	std::vector<std::string> result;
	std::string path = "";
	if (uri[uri.size() - 1] != '/') {
		int last = uri.find_last_of('/');
		file.push_back(uri.substr(last + 1));
		path = uri.substr(0, last + 1);
	}
	else {
		path = uri;
	}
	if (shared::string::splitPath(path, result) == 1) {
		return 1;
	}
	config::t_location temp;
	temp.locations = locations;
	bool deeper = false;
	int len = 0;
	while (temp.locations.size() > 0) {
		bool found = false;
		for (std::vector<config::t_location>::const_iterator it = temp.locations.begin(); it != temp.locations.end(); ++it) {
			if (comparePaths(it->path, result, len) == 0) {
				temp = *it;
				found = true;
				break;
			}
		}
		if (found == false) {
			if (deeper == false) {
				return 1;
			}
			else {
				break;
			}
		}
		deeper = true;
	}
	if (!file.empty()) {
		temp.index = file;
	}
	if (temp.root != "") {
		while (len > 0) {
			result.erase(result.begin());
			len--;
		}
		if (temp.root[temp.root.size() - 1] != '/'){
			temp.root += "/";
		}
	}
	for (std::vector<std::string>::const_iterator it = result.begin(); it != result.end(); ++it) {
		temp.root += *it + "/";
	}
	location = temp;
	if (uri[uri.size() - 1] != '/')
	{
		if (file[0].substr(file[0].find_last_of('.') + 1) == "php")
			m_type = CGI;
	}
	return 0;
}

/**
 * @brief prints a location.
 * @param locations the locations to print.
 * @param detailed toggles between detailed and simple output. (0 for simple, 1
 * for detailed)
 */
void Router::printLocation(const config::t_location location, int detailed) {
	std::cout << "--------------------------" << std::endl;
	std::cout << "Location: " ;
	//std::cout << " name: " << locations[j].name << std::endl;
	for (std::vector<std::string>::const_iterator it = location.path.begin();
		 it != location.path.end(); ++it) {
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
