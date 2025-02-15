#pragma once

#include "http/types.hpp"

#include <set>
#include <string>
#include <vector>

namespace config {
	struct Location {
			std::vector<std::string> path;				// location name split into tokens
			std::vector<std::string> root;				// root split into tokens // TODO: maybe actually assign root to parent root in parser, if not specified in config? might make things easier
			std::string redirectUri;					// return route (references other locations)
			std::vector<std::string> redirectUriTokens; // return route split into tokens
			std::set<http::Method> allowedMethods;		// allowed HTTP methods
			std::string uploadSubdirectory;				// subdirectory for uploads
			bool autoindex;								//
			std::vector<std::string> indexFile;			// file to load when GETting a directory
			std::vector<Location> locations;			// registered locations // TODO: maybe rename to 'children'

			// default constructor
			Location()
				: path()
				, root()
				, redirectUri()
				, redirectUriTokens()
				, allowedMethods()
				, uploadSubdirectory()
				, autoindex(false)
				, indexFile()
				, locations() {
			}

			// copy constructor
			Location(const Location& other)
				: path(other.path)
				, root(other.root)
				, redirectUri(other.redirectUri)
				, redirectUriTokens(other.redirectUriTokens)
				, allowedMethods(other.allowedMethods)
				, uploadSubdirectory(other.uploadSubdirectory)
				, autoindex(other.autoindex)
				, indexFile(other.indexFile)
				, locations(other.locations) {
			}

			// assignment operator
			Location& operator=(const Location& other) {
				if (this != &other) {
					path = other.path;
					root = other.root;
					redirectUri = other.redirectUri;
					redirectUriTokens = other.redirectUriTokens;
					allowedMethods = other.allowedMethods;
					uploadSubdirectory = other.uploadSubdirectory;
					autoindex = other.autoindex;
					indexFile = other.indexFile;
					locations = other.locations;
				}
				return *this;
			}

			bool acceptsFileUpload() const {
				return !uploadSubdirectory.empty() && allowedMethods.find(http::POST) != allowedMethods.end();
			}

			bool hasRedirect() const {
				return !redirectUri.empty();
			}

			/**
			 * @brief Validate the Location
			 */
			void validate() const {
				// throw config::parse_exception(1, "consarnit");
			}

			// void reset() {
			// 	path.clear();
			// 	root.clear();
			// 	redirectUri.clear();
			// 	redirectUriTokens.clear();
			// 	allowedMethods.clear();
			// 	uploadSubdirectory.clear();
			// 	autoindex = false;
			// 	indexFile.clear();
			// 	locations.clear();
			// }
	};
} // namespace config
