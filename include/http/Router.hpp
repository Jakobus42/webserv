#pragma once

#include "config/Parser.hpp"
#include "http/Request.hpp"

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
			int findLocation(const std::string& uri, config::Location& location); // currently not implemented
			StatusCode fileExists(const std::string& absoluteFilePath);
			void printLocation(const config::Location& location, int detailed); // replace with operator<< overload?
			std::string getSafePath(const Uri& uri);

		private:
			Router();

			std::string getBaseRoot(const Uri& uri);
			void splitPath(const std::string& path, std::vector<std::string>& tokens);
			std::string normalizePath(const std::string& uriPath);
			const config::Location* locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs);

		private:
			std::vector<config::Location> m_locations;
			config::Location m_globalRoot;
			std::string m_dataDir;
			static const int MAX_REDIRECTS = 32;
	};

} /* namespace http */
