#pragma once

#include "http/http.hpp"

#include <set>
#include <vector>

namespace config {

	struct LocationConfig {
			std::string path;									 // location name split into tokens
			std::vector<std::string> pathAsTokens;				 // location name split into tokens
			std::string root;									 // root split into tokens
			std::string precalculatedAbsolutePath;				 // absolute path to this location's root, pre-calculated after parsing
			std::vector<std::string> rootAsTokens;				 // root split into tokens
			std::string redirectUri;							 // return route (references other locations)
			std::vector<std::string> redirectUriAsTokens;		 // return route split into tokens
			std::set<http::Method> allowedMethods;				 // allowed HTTP methods
			std::string uploadSubdirectory;						 // subdirectory for uploads
			std::vector<std::string> uploadSubdirectoryAsTokens; //
			bool autoindex;										 //
			std::vector<std::string> indexFile;					 // files to load when GETting a directory
			std::vector<LocationConfig> locations;				 // registered locations

			LocationConfig();
			LocationConfig(const LocationConfig& other);
			LocationConfig& operator=(const LocationConfig& other);

			bool acceptsFileUpload() const;
			bool hasRedirect() const;
			bool hasOwnRoot() const;

			void validate() const;

			void printIndented(int indentLevel) const;
			void print() const;
	};

} // namespace config
