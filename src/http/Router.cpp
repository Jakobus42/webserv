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
	while (paths < path.size())
	{
		if (path[paths] != pathToCompare[paths])
		{
			return 1;
		}
		paths++;
	}
	return 0;
}

int Router::getLocation(const std::string& uri, std::vector<config::t_location>& locations, config::t_location* location) {
	//returns: 0 if successful, 1 if error
	std::cout << "Router: getLocation: " << uri << std::endl;
	std::string file = "";
	std::string path = "";
	(void)locations;
	std::vector<std::string> result;
	if (uri[uri.size() - 1] != '/') {
		int last = uri.find_last_of('/');
		file = uri.substr(last + 1);
		path = uri.substr(0, last + 1);
	}
	else {
		path = uri;
	}
	if (shared::string::splitPath(path, result) == 1) {
		return 1;
	}
	config::t_location& temp = (*locations)[0];
	while (temp.locations.size() > 0) {
		int found = 0;
		for (std::vector<config::t_location>::const_iterator it = temp.locations.begin(); it != temp.locations.end(); ++it) {
			//for (std::vector<std::string>::const_iterator it2 = it->path.begin(); it2 != it->path.end(); ++it2) {
			if (comparePaths(result, it->path) == 0) {
				temp = *it;
				found = 1;
				break;
			}			
		}
		if (found == 0) {
			break;
		}
	}
	if (file.empty()) {
		file = 
	}
	return 0;
}

} /* namespace http */
