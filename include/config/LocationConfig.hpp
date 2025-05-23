#pragma once

#include "http/http.hpp"

#include <map>
#include <set>
#include <vector>

namespace config {

	struct LocationConfig {
			std::string path;									  // location name split into tokens
			std::string root;									  // root split into tokens
			std::pair<http::StatusCode, std::string> redirectUri; // return route (references other locations)
			std::set<http::Method> allowedMethods;				  // allowed HTTP methods
			std::string uploadSubdirectory;						  // subdirectory for uploads
			bool autoindex;										  //
			bool isServerRoot;									  //
			std::vector<std::string> indexFile;					  // files to load when GETting a directory
			std::map<http::StatusCode, std::string> errorPages;	  //
			std::vector<LocationConfig> locations;				  // registered locations

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
