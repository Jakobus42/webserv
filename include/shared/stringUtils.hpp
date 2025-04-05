#pragma once

#include <sstream>

namespace shared {

	namespace string {

		// only supports base 16 and 10
		template <typename T>
		T toNum(const std::string& str, std::size_t base) {
			T result;
			std::istringstream stream(str);

			if (base == 16) {
				stream >> std::hex;
			} else if (base == 8) {
				stream >> std::oct;
			}
			stream >> result;
			if (stream.fail() || !stream.eof())
				throw std::runtime_error("cant convert to number");
			return result;
		}

		template <typename T>
		std::string toString(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		// todo: move this somewhere else
		//  /**
		//   * @brief Splits a path into its components, delimited by '/'
		//   *
		//   * @param path The path to split
		//   * @return std::vector<std::string> The individual path components
		//   * @throws HttpException if the path is invalid
		//   */
		//  std::vector<std::string> splitPath(const std::string& path) throw(HttpException);

		// /**
		//  * @brief Joins a split path into a string, delimited by '/'
		//  *        Should never receive broken data
		//  *
		//  * @param pathComponents The components to join
		//  * @return std::string The composed path
		//  */
		// std::string joinPath(const std::vector<std::string>& pathComponents);

		// /**
		//  * @brief Normalizes a path against a basePath
		//  *        Paths can be relative (use ./ and ../), but may not
		//  *        go lower than the basePath.
		//  *
		//  * @param path The path to normalize split into components by splitPath()
		//  * @param basePath The root path which the path may not escape, split by splitPath()
		//  * @return std::vector<std::string> The normalized path, without any "./" or "../"
		//  */
		// std::vector<std::string> normalizePath(const std::vector<std::string>& path);

		// /**
		//  * @brief Normalizes a path against a basePath, from and to a string
		//  *
		//  * @param path
		//  * @return std::string
		//  */
		// std::string normalizePath(const std::string& path);


	} // namespace string

} // namespace shared