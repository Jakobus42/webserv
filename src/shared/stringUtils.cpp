#include "shared/stringUtils.hpp"

namespace shared {
	namespace string {

		std::vector<std::string> split(const std::string& str, char delimiter) {
			std::vector<std::string> tokens;
			std::stringstream ss(str);
			std::string segment;

			while (std::getline(ss, segment, delimiter)) {
				if (!segment.empty()) {
					tokens.push_back(segment);
				}
			}
			return tokens;
		}

		// todo: move this somewhere else

		//  std::string joinPath(const std::vector<std::string>& pathComponents) {
		//  	std::string joined = "";
		//  	for (std::vector<std::string>::const_iterator it = pathComponents.begin(); it != pathComponents.end(); ++it) {
		//  		joined += "/";
		//  		joined += *it;
		//  	}
		//  	return joined;
		//  }

		// std::vector<std::string> normalizePath(const std::vector<std::string>& path) {
		// 	std::vector<std::string> normalized;

		// 	for (std::vector<std::string>::const_iterator it = path.begin(); it != path.end(); ++it) {
		// 		if (*it == "..") {
		// 			if (!normalized.empty()) {
		// 				normalized.pop_back();
		// 			}
		// 		} else if (!it->empty() && *it != ".") {
		// 			normalized.push_back(*it);
		// 		}
		// 	}
		// 	return normalized;
		// }

		// std::string normalizePath(const std::string& path) {
		// 	return joinPath(normalizePath(splitPath(path))); // TODO: will turn "/" and "/////" into an empty string
		// }


	} // namespace string

} // namespace shared
