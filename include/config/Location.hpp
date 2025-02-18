#pragma once

#include "http/types.hpp"

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace config {
	struct Location {
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
			std::vector<Location> locations;					 // registered locations

			// default constructor
			Location();

			// copy constructor
			Location(const Location& other);

			// assignment operator
			Location& operator=(const Location& other);

			bool acceptsFileUpload() const;
			bool hasRedirect() const;
			bool hasOwnRoot() const;

			/**
			 * @brief Validate the Location
			 */
			void validate() const;

			/**
			 * @brief Helper to print this location with indentation.
			 * @param indentLevel Number of spaces to indent.
			 */
			void printIndented(int indentLevel) const;

			/**
			 * @brief Prints the Location details.
			 */
			void print() const;

			// void reset() {
			// 	path.clear();
			// 	root.clear();
			// 	redirectUri.clear();
			// 	redirectUriAsTokens.clear();
			// 	allowedMethods.clear();
			// 	uploadSubdirectory.clear();
			// 	autoindex = false;
			// 	indexFile.clear();
			// 	locations.clear();
			// }
	};
} // namespace config
