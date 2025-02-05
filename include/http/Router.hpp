#pragma once

#include "config/Parser.hpp"
#include "http/types.hpp"

#include <string>
#include <vector>

namespace http {
	class Router {
		public:
			Router(const std::vector<config::Location>& locations, const config::Location& globalRoot);
			~Router();
			Router(const Router& other);
			const Router& operator=(const Router& rhs);
			const config::Location& getGlobalRoot() const;

		public:
			/**
			 * @brief Checks whether a path exists in the file system, and whether it is a file or directory
			 *
			 * @param absolutePath The absolute path to check
			 * @return FileType to indicate the type: FILE, DIRECTORY or _NOT_FOUND if the path doesn't exist
			 */
			static FileType checkFileType(const std::string& absolutePath);

			/**
			 * @brief Splits a path into its components, delimited by '/'
			 *
			 * @param path The path to split
			 * @return std::vector<std::string> The individual path components
			 * @throws http::exception if the path is invalid
			 */
			static std::vector<std::string> splitPath(const std::string& path) throw(http::exception);

			/**
			 * @brief Joins a split path into a string, delimited by '/'
			 *        Should never receive broken data
			 *
			 * @param pathComponents The components to join
			 * @return std::string The composed path
			 */
			static std::string joinPath(const std::vector<std::string>& pathComponents);

			/**
			 * @brief Normalizes a path against a basePath (globalRoot by default)
			 *        Paths can be relative (use ./ and ../), but may not
			 *        go lower than the basePath.
			 *
			 * @param path The path to normalize split into components by splitPath()
			 * @param basePath The root path which the path may not escape, split by splitPath()
			 * @return std::vector<std::string> The normalized path, without any "./" or "../"
			 */
			static std::vector<std::string> normalizePath(const std::vector<std::string>& path);

			/**
			 * @brief Generate the absolute path for a location and an optional subPath
			 *
			 * @param location The location of which to retrieve the root path
			 * @param subPath The subdirectory to retrieve in that root path
			 * @return std::string The absolute path generated from  <location.path>/<subPath>
			 */
			std::string findAbsolutePath(const config::Location& location, const std::string& subPath = "");

			/**
			 * @brief Traverses m_locations according to a Request's Uri,
			 *        respecting redirections while avoiding redirect loops
			 *
			 * @param uri The URI requested by the client
			 * @return std::string The absolute, normalized path to a file or directory
			 * @throws http::exception upon any issues encountered during routing
			 */
			std::pair<std::string, const config::Location*> routeToPath(const std::vector<std::string>& uriPath, const config::Location& currentLocation, const std::vector<std::string>& currentRootPath, std::size_t redirects = 0, std::size_t depth = 0) throw(http::exception);

			/**
			 * Router needs to:
			 *
			 * - Normalize paths versus a certain base path
			 *   -> normalizePath();
			 *
			 * - Split a path into its components
			 *   and (?) validate whether it is even a valid path
			 *   -> splitPath();
			 *
			 * - Construct and return the absolute path from a Location,
			 *   the rest of the uri that wasn't consumed while routing,
			 *   and the global base path that should be generated on startup
			 *   -> generateAbsolutePath();
			 *
			 * - Identify whether an absolute path exists in the file system,
			 *   whether it is a file or a directory, and whether
			 *   that file or directory is accessible
			 *
			 * - Traverse redirects as part of the routing process,
			 *   detect loops caused by circular redirects and
			 *   limit total redirects
			 *
			 *
			 * This may be achieved by storing all components of the uri path
			 * in something like a queue of strings, then looking for a match
			 * of the first component in locations. If found,
			 * remove that component from the queue, set the currentLocation
			 * to that location's root, and look for the next component in
			 * that location's children.
			 *
			 * Once a location doesn't have a child matching the next component,
			 * or when the last component is reached, store
			 * any remaining components as the subdirectory. Then, look
			 * for that subdirectory (and / or file) in the current Location's
			 * root path. All Location's root paths are relative to the globalRoot.
			 * If a Location doesn't have a root path (or the root path is empty),
			 * just use the globalRoot Location instead.
			 *
			 * TODO: Parser -> The globalRoot should already contain the dataDir.
			 * e.g.: data_dir = /www and global_root = /var/server in config,
			 * the globalRoot Location should have the path /var/server/www.
			 *
			 */

		private:
			static const std::size_t MAX_REDIRECTS = 32;

			std::vector<config::Location> m_locations;
			config::Location m_globalRoot;
	};
} // namespace http
