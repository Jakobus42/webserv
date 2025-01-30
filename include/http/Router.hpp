#pragma once

#include "config/Parser.hpp"
#include "http/Request.hpp"

namespace http {

	/**
	 * @class Router
	 * @brief ...
	 */
	class Router : public shared::NonCopyable {
		public:
			Router();
			~Router();

			static const config::Location& getLocation(const http::Uri& uri);

			static StatusCode fileExists(const std::string& filePath);

			static bool splitPath(const std::string& path, std::vector<std::string>& tokens);

			static std::string buildFinalPath(const std::string& baseRoot, const std::string& normUri);

			static const config::Location* locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs);


		private:
			static std::vector<config::Location> m_locations;
			static config::Location m_defaultLocation;
	};

} /* namespace http */
