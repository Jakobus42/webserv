#include "http/Router.hpp"

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

int comparePaths(const std::vector<std::string>& path, const std::vector<std::string>& pathToCompare) {
	unsigned long paths = 0;
	if (path.size() > pathToCompare.size())
	{
		return 1;
	}
	while (paths < pathToCompare.size())
	{
		if (path.size() <= paths)
		{
			return 1;
		}
		if (path[paths] != pathToCompare[paths])
		{
			return 1;
		}
		paths++;
	}
	return 0;
}

int Router::findLocation(const std::string& uri, std::vector<config::t_location>& locations, config::t_location& location) {
	std::cout << "--------------------------------------------" << std::endl;
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
	std::cout << "path: " << path << std::endl;
	if (shared::string::splitPath(path, result) == 1) {
		std::cerr << "Router: findLocation: splitPath failed" << std::endl;
		return 1;
	}
	config::t_location temp;
	temp.locations = locations;
	while (temp.locations.size() > 0) {
		bool found = false;
		//TODO: find some way to break, when the result is the same as the location path
		
		for (std::vector<config::t_location>::const_iterator it = temp.locations.begin(); it != temp.locations.end(); ++it) {
			//print
/* 			std:: cout << "comparing with location: " << std::endl;
			for (std::vector<std::string>::const_iterator it2 = it->path.begin(); it2 != it->path.end(); ++it2) {
					std::cout << *it2 << "/";
				}
			std::cout << std::endl; */
			//print
			std::cout << "Router: findLocation: comparing with location: " << std::endl;
			if (comparePaths(result, it->path) == 0) {
				temp = *it;
				found = true;
				std::cout << "Router: findLocation: found location" << std::endl;
				break;
			}
		}
		if (found == false) {
			std::cout << "Router: findLocation: location not found" << std::endl;
			return 1;
		}
	}
	if (!file.empty()) {
		temp.index = file;
	}
	location = temp;
	std::cout << "Router: findLocation: location: " << std::endl;
	for (std::vector<std::string>::const_iterator it = location.path.begin(); it != location.path.end(); ++it) {
		std::cout << *it << "/";
	}
	std::cout << std::endl;
	std::cout << "--------------------------------------------" << std::endl;
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
		std::cout << "Path: " << *it << std::endl;
	}
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
