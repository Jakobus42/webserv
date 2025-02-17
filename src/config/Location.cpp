#include "config/Location.hpp"

namespace config {

	Location::Location()
		: path()
		, root()
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

	// copy constructor
	Location::Location(const Location& other)
		: path(other.path)
		, root(other.root)
		, redirectUri(other.redirectUri)
		, redirectUriAsTokens(other.redirectUriAsTokens)
		, allowedMethods(other.allowedMethods)
		, uploadSubdirectory(other.uploadSubdirectory)
		, uploadSubdirectoryAsTokens(other.uploadSubdirectoryAsTokens)
		, autoindex(other.autoindex)
		, indexFile(other.indexFile)
		, locations(other.locations) {
	}

	// assignment operator
	Location& Location::operator=(const Location& other) {
		if (this != &other) {
			path = other.path;
			root = other.root;
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

	bool Location::hasRedirect() const {
		return !redirectUri.empty();
	}

	/**
	 * @brief Validate the Location
	 */
	void Location::validate() const {
		// throw config::parse_exception(1, "consarnit");
	}

	void Location::printIndented(int indentLevel) const {
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
			for (std::vector<std::string>::const_iterator it = rootAsTokens.begin(); it != rootAsTokens.end(); ++it) {
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

	void Location::print() const {
		printIndented(0);
	}

} // namespace config