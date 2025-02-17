#pragma once

#include <stdint.h>

#include "http/http.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace shared {

	namespace string {

		const std::string WHITESPACES = " \t\n\v\f\r";

		void split(const std::string& str, std::vector<std::string>& result, const std::string& delimiters = WHITESPACES);

		void trim(std::string& str, const std::string& delimiters = WHITESPACES);

		uint32_t StoiHex(std::string& str, int& ret);

		template <typename T>
		T toNum(const std::string& str, int base) {
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

		bool isGreyspace(char c);
		std::string fromNum(int32_t number);

		template <typename T>
		std::string to_string(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

		/**
		 * @brief Splits a path into its components, delimited by '/'
		 *
		 * @param path The path to split
		 * @return std::vector<std::string> The individual path components
		 * @throws http::exception if the path is invalid
		 */
		std::vector<std::string> splitPath(const std::string& path) throw(http::exception);

		/**
		 * @brief Joins a split path into a string, delimited by '/'
		 *        Should never receive broken data
		 *
		 * @param pathComponents The components to join
		 * @return std::string The composed path
		 */
		std::string joinPath(const std::vector<std::string>& pathComponents);

		/**
		 * @brief Normalizes a path against a basePath
		 *        Paths can be relative (use ./ and ../), but may not
		 *        go lower than the basePath.
		 *
		 * @param path The path to normalize split into components by splitPath()
		 * @param basePath The root path which the path may not escape, split by splitPath()
		 * @return std::vector<std::string> The normalized path, without any "./" or "../"
		 */
		std::vector<std::string> normalizePath(const std::vector<std::string>& path);

		/**
		 * @brief Normalizes a path against a basePath, from and to a string
		 *
		 * @param path
		 * @return std::string
		 */
		std::string normalizePath(const std::string& path);


	} // namespace string

} // namespace shared