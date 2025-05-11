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

		std::string join(const std::vector<std::string>& vec, const std::string& separator) {
			std::string joined;

			for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
				if (it != vec.begin()) {
					joined += separator;
				}
				joined += *it;
			}

			return joined;
		}

	} // namespace string

} // namespace shared
