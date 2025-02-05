#pragma once

#include "http/types.hpp"

#include <set>
#include <string>
#include <vector>

namespace config {
	struct Location {
			std::vector<std::string> path;				// location name split into tokens
			std::vector<std::string> root;				// root split into tokens
			std::string redirectUri;					// return route (references other locations)
			std::vector<std::string> redirectUriTokens; // return route split into tokens
			std::set<http::Method> allowedMethods;		// allowed HTTP methods
			bool autoindex;								//
			std::vector<std::string> indexFile;			// file to load when GETting a directory
			std::vector<Location> locations;			// registered locations // TODO: maybe rename to 'children'

			// globalRoot should always have path <global_root>/<data_dir>/
			// what about defaults for accepted methods?
			// should globalRoot even be a Location?
			// or just a string that's used as a base for all routes?

			// default constructor
			Location()
				: path()
				, root()
				, redirectUri()
				, redirectUriTokens()
				, allowedMethods()
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
					autoindex = other.autoindex;
					indexFile = other.indexFile;
					locations = other.locations;
				}
				return *this;
			}

			bool acceptsFileUpload() const {
				return allowedMethods.find(http::POST) != allowedMethods.end();
			}

			bool hasRedirect() const {
				return !redirectUri.empty();
			}
	};
} // namespace config
