#pragma once

#include "http/http.hpp"

#include <set>
#include <vector>
#include <map>

namespace config {

	struct LocationConfig {
			std::string path;					   // location name split into tokens
			std::string root;					   // root split into tokens
			std::string precalculatedAbsolutePath; // absolute path to this location's root, pre-calculated after parsing
			std::string redirectUri;			   // return route (references other locations)
			http::StatusCode returnClass;		   // type of redirect
			std::set<http::Method> allowedMethods; // allowed HTTP methods
			std::string uploadSubdirectory;		   // subdirectory for uploads
			bool autoindex;						   //
			std::vector<std::string> indexFile;	   // files to load when GETting a directory
			std::map<int, std::string> errorPages; 
			std::vector<LocationConfig> locations; // registered locations

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
