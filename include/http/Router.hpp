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
			Router(const std::vector<config::Location>& locations);
			~Router();
			Router(const Router& other);
			const Router& operator=(const Router& rhs);

		public:
			const config::Location& getLocation(const http::Uri& uri);
			int findLocation(const std::string& uri, config::Location& location); // currently not implemented
			StatusCode fileExists(const std::string& absoluteFilePath);
			void printLocation(const config::Location& location, int detailed); // replace with operator<< overload?


		private:
			Router();

			void splitPath(const std::string& path, std::vector<std::string>& tokens);
			const config::Location* locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs);
			std::string buildFinalPath(const std::string& baseRoot, const std::string& normUri);

		private:
			std::vector<config::Location> m_locations;
			config::Location m_defaultLocation;
			static const int MAX_REDIRECTS = 32;
	};

} /* namespace http */
