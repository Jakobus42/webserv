#pragma once

#include "config/Parser.hpp"
#include "http/Request.hpp"
#include "http/types.hpp"

namespace http {

	/**
	 * @class Router
	 * @brief ...
	 */
	class Router {
		public:
			Router(const std::vector<config::Location>& locations, const config::Location& globalRoot, const std::string& dataDir);
			~Router();
			Router(const Router& other);
			const Router& operator=(const Router& rhs);

		public:
			const config::Location& getLocation(const http::Uri& uri);
			std::string getSafePath(const Uri& uri, const config::Location& matchedLocation);
			FileType checkFileType(const std::string& absoluteFilePath);
			void printLocation(const config::Location& location, int detailed); // replace with operator<< overload?

		private:
			Router();

			std::string getBaseRoot(const Uri& uri);
			void splitPath(const std::string& path, std::vector<std::string>& tokens);
			std::string normalizePath(const std::string& uriPath);
			const config::Location* locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs);

		private:
			static const int MAX_REDIRECTS = 32;

			std::vector<config::Location> m_locations;
			config::Location m_globalRoot;
			std::string m_dataDir;
	};

} /* namespace http */
