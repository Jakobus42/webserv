#include "config/Location.hpp"

#include "config/Parser.hpp"

namespace config {

	Location::Location()
		: path()
		, pathAsTokens()
		, root()
		, precalculatedAbsolutePath()
		, redirectUri()
		, redirectUriAsTokens()
		, allowedMethods()
		, uploadSubdirectory()
		, uploadSubdirectoryAsTokens()
		, autoindex(false)
		, indexFile()
		, locations() {
		allowedMethods.insert(http::GET);
		allowedMethods.insert(http::POST);
		allowedMethods.insert(http::DELETE);
	}

	Location::Location(const Location& other)
		: path(other.path)
		, pathAsTokens(other.pathAsTokens)
		, root(other.root)
		, precalculatedAbsolutePath(other.precalculatedAbsolutePath)
		, redirectUri(other.redirectUri)
		, redirectUriAsTokens(other.redirectUriAsTokens)
		, allowedMethods(other.allowedMethods)
		, uploadSubdirectory(other.uploadSubdirectory)
		, uploadSubdirectoryAsTokens(other.uploadSubdirectoryAsTokens)
		, autoindex(other.autoindex)
		, indexFile(other.indexFile)
		, locations(other.locations) {
	}

	Location& Location::operator=(const Location& other) {
		if (this != &other) {
			path = other.path;
			pathAsTokens = other.pathAsTokens;
			root = other.root;
			precalculatedAbsolutePath = other.precalculatedAbsolutePath;
			redirectUri = other.redirectUri;
			redirectUriAsTokens = other.redirectUriAsTokens;
			allowedMethods = other.allowedMethods;
			uploadSubdirectory = other.uploadSubdirectory;
			uploadSubdirectoryAsTokens = other.uploadSubdirectoryAsTokens;
			autoindex = other.autoindex;
			indexFile = other.indexFile;
			locations = other.locations;
		}
		return *this;
	}

	bool Location::acceptsFileUpload() const {
		return !uploadSubdirectory.empty() && allowedMethods.find(http::POST) != allowedMethods.end();
	}

	// TODO: validate?
	bool Location::hasRedirect() const {
		return !redirectUri.empty();
	}

	bool Location::hasOwnRoot() const {
		return !root.empty();
	}

	// TODO: implement
	void Location::validate() const {
		if (path.empty()) {
			throw parse_exception("Location doesn't have a path");
		}
	}

	void Location::printIndented(int indentLevel) const {
		std::string indent(indentLevel, ' ');
		// Print the location header line
		std::cout << indent << "Location: ";
		if (!pathAsTokens.empty()) {
			for (std::vector<std::string>::const_iterator it = pathAsTokens.begin(); it != pathAsTokens.end(); ++it) {
				std::cout << "/" << *it;
			}
		} else {
			std::cout << "/";
		}
		std::cout << std::endl;

		if (!rootAsTokens.empty()) {
			std::cout << indent << "  Root: ";
			for (std::vector<std::string>::const_iterator it = rootAsTokens.begin(); it != rootAsTokens.end(); ++it) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}

		if (!root.empty()) {
			std::cout << indent << "  Root (string): " << root << std::endl;
		}

		if (!precalculatedAbsolutePath.empty()) {
			std::cout << indent << "  Absolute path: " << precalculatedAbsolutePath << std::endl;
		}

		if (!redirectUri.empty()) {
			std::cout << indent << "  Redirect URI: " << redirectUri << std::endl;
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

		if (!indexFile.empty()) {
			std::cout << indent << "  Index Files: ";
			for (std::vector<std::string>::const_iterator it = indexFile.begin();
				 it != indexFile.end();
				 ++it) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}

		if (!locations.empty()) {
			std::cout << indent << "  Nested Locations:" << std::endl;
			for (std::vector<Location>::const_iterator it = locations.begin();
				 it != locations.end();
				 ++it) {
				it->printIndented(indentLevel + 4);
			}
		}
	}

	void Location::print() const {
		printIndented(0);
	}

} // namespace config