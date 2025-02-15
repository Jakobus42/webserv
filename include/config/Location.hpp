#pragma once

#include "http/types.hpp"

#include <iostream>
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
				allowedMethods.insert(http::GET);
				allowedMethods.insert(http::POST);
				allowedMethods.insert(http::DELETE);
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

			/**
			 * @brief Helper to print this location with indentation.
			 * @param indentLevel Number of spaces to indent.
			 */
			void printIndented(int indentLevel) const {
				std::string indent(indentLevel, ' ');
				// Print the location header line
				std::cout << indent << "Location: ";
				if (!path.empty()) {
					for (std::vector<std::string>::const_iterator it = path.begin();
						 it != path.end();
						 ++it) {
						std::cout << "/" << *it;
					}
				} else {
					std::cout << "/";
				}
				std::cout << std::endl;

				// Print root if specified.
				if (!root.empty()) {
					std::cout << indent << "  Root: ";
					for (std::vector<std::string>::const_iterator it = root.begin(); it != root.end(); ++it) {
						std::cout << *it << " ";
					}
					std::cout << std::endl;
				}

				// Print redirect URI if specified.
				if (!redirectUri.empty()) {
					std::cout << indent << "  Redirect URI: " << redirectUri << std::endl;
				}

				// Print allowed methods.
				std::cout << indent << "  Allowed Methods: ";
				for (std::set<http::Method>::const_iterator it = allowedMethods.begin();
					 it != allowedMethods.end();
					 ++it) {
					std::cout << *it << " ";
				}
				std::cout << std::endl;

				// Print upload subdirectory if set.
				if (!uploadSubdirectory.empty()) {
					std::cout << indent << "  Upload Subdirectory: " << uploadSubdirectory << std::endl;
				}

				// Print autoindex state.
				std::cout << indent << "  Autoindex: " << (autoindex ? "on" : "off") << std::endl;

				// Print index files if any.
				if (!indexFile.empty()) {
					std::cout << indent << "  Index Files: ";
					for (std::vector<std::string>::const_iterator it = indexFile.begin();
						 it != indexFile.end();
						 ++it) {
						std::cout << *it << " ";
					}
					std::cout << std::endl;
				}

				// Print nested locations recursively, indenting them further.
				if (!locations.empty()) {
					std::cout << indent << "  Nested Locations:" << std::endl;
					for (std::vector<Location>::const_iterator it = locations.begin();
						 it != locations.end();
						 ++it) {
						it->printIndented(indentLevel + 4);
					}
				}
			}

			/**
			 * @brief Prints the Location details.
			 */
			void print() const {
				printIndented(0);
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
