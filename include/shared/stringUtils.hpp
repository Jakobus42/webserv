#pragma once

#include <sstream>
#include <vector>

namespace shared {
	namespace string {
		struct CaseInsensitiveComparator {
				bool operator()(const std::string& s1, const std::string& s2) const;
		};

		template <typename T>
		T toNum(const std::string& str, std::ios_base& (*baseManipulator)(std::ios_base&) = NULL) {
			T result;
			std::istringstream stream(str);

			if (baseManipulator) {
				stream >> baseManipulator;
			}

			stream >> result;
			if (stream.fail()) {
				throw std::runtime_error("cant convert to number");
			}
			return result;
		}

		template <typename T>
		std::string toString(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		std::vector<std::string> split(const std::string& str, char delimiter);

		std::string join(const std::vector<std::string>& vec, const std::string& separator);

	} // namespace string
} // namespace shared