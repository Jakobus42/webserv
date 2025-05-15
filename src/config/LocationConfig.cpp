#include "config/LocationConfig.hpp"

#include "config/ParseException.hpp"

#include <iostream>

namespace config {

	LocationConfig::LocationConfig()
		: path()
		, root()
		, precalculatedAbsolutePath()
		, redirectUri()
		, allowedMethods()
		, uploadSubdirectory()
		, autoindex(false)
		, isServerRoot(false)
		, indexFile()
		, errorPages()
		, locations() {
		allowedMethods.insert(http::GET);
		allowedMethods.insert(http::POST);
		allowedMethods.insert(http::DELETE);
	}

	LocationConfig::LocationConfig(const LocationConfig& other)
		: path(other.path)
		, root(other.root)
		, precalculatedAbsolutePath(other.precalculatedAbsolutePath)
		, redirectUri(other.redirectUri)
		, allowedMethods(other.allowedMethods)
		, uploadSubdirectory(other.uploadSubdirectory)
		, autoindex(other.autoindex)
		, isServerRoot(other.isServerRoot)
		, indexFile(other.indexFile)
		, errorPages(other.errorPages)
		, locations(other.locations) {
	}

	LocationConfig& LocationConfig::operator=(const LocationConfig& other) {
		if (this != &other) {
			path = other.path;
			root = other.root;
			precalculatedAbsolutePath = other.precalculatedAbsolutePath;
			redirectUri = other.redirectUri;
			allowedMethods = other.allowedMethods;
			uploadSubdirectory = other.uploadSubdirectory;
			autoindex = other.autoindex;
			isServerRoot = other.isServerRoot;
			indexFile = other.indexFile;
			errorPages = other.errorPages;
			locations = other.locations;
		}
		return *this;
	}

	bool LocationConfig::acceptsFileUpload() const {
		return !uploadSubdirectory.empty() && allowedMethods.find(http::POST) != allowedMethods.end();
	}

	// TODO: validate?
	bool LocationConfig::hasRedirect() const {
		return !redirectUri.second.empty();
	}

	bool LocationConfig::hasOwnRoot() const {
		return !root.empty();
	}

	// TODO: implement
	void LocationConfig::validate() const {
		if (path.empty()) {
			throw ParseException("LocationConfig doesn't have a path");
		}
	}

	void LocationConfig::printIndented(int indentLevel) const {
		std::string indent(indentLevel, ' ');

		std::cout << indent << "LocationConfig: " << path << std::endl;

		if (!root.empty()) {
			std::cout << indent << "  Root (string): " << root << std::endl;
		}

		if (!precalculatedAbsolutePath.empty()) {
			std::cout << indent << "  Absolute path: " << precalculatedAbsolutePath << std::endl;
		}

		if (!redirectUri.second.empty()) {
			std::cout << indent << "  Redirect URI: " << redirectUri.second << std::endl;
		}

		std::cout << indent << "  Allowed Methods: ";
		for (std::set<http::Method>::const_iterator it = allowedMethods.begin();
			 it != allowedMethods.end();
			 ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;

		if (!uploadSubdirectory.empty()) {
			std::cout << indent << "  Upload Subdirectory: " << uploadSubdirectory << std::endl;
		}

		std::cout << indent << "  Autoindex: " << (autoindex ? "on" : "off") << std::endl;
		std::cout << indent << "  isServerRoot: " << (isServerRoot ? "true" : "false") << std::endl;

		if (!indexFile.empty()) {
			std::cout << indent << "  Index Files: ";
			for (std::vector<std::string>::const_iterator it = indexFile.begin();
				 it != indexFile.end();
				 ++it) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}

		if (!errorPages.empty()) {
			std::cout << indent << "  Error Pages:" << std::endl;
			for (std::map<http::StatusCode, std::string>::const_iterator it = errorPages.begin();
				 it != errorPages.end();
				 ++it) {
				std::cout << indent << "    " << it->first << ": " << it->second << std::endl;
			}
		}

		if (!locations.empty()) {
			std::cout << indent << "  Nested Locations:" << std::endl;
			for (std::vector<LocationConfig>::const_iterator it = locations.begin();
				 it != locations.end();
				 ++it) {
				it->printIndented(indentLevel + 4);
			}
		}
	}

	void LocationConfig::print() const {
		printIndented(0);
	}

} // namespace config