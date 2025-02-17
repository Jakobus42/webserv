#include <sstream>
#include <string>
#include <vector>

namespace shared {
	namespace string {
		/**
		 * @brief Splits a path into its components.
		 *
		 * This function splits a path into its components, which are separated by
		 * forward slashes. The components are stored in the `result` vector.
		 *
		 * @param path The path to split.
		 * @param result A vector to store the components of the path.
		 *
		 * @return 0 if the path was successfully split, 1 if the path is invalid.
		 */
		int splitPath(const std::string& path, std::vector<std::string>& result) {
			std::vector<std::string> tempResult;
			std::string tmp = path;
			if (tmp[0] != '/') {
				return 1;
			}
			std::stringstream ss(tmp.substr(1));
			std::string segment;
			while (std::getline(ss, segment, '/')) {
				if (!segment.empty()) {
					tempResult.push_back(segment);
				} else {
					return 1;
				}
			}
			result = tempResult;
			return 0;
		}
	} // namespace string
} // namespace shared