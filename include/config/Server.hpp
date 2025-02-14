#pragma once

#include "config/Location.hpp"
#include "http/http.hpp"
#include "http/types.hpp"

#include <map>
#include <string>
#include <vector>

namespace config {

	static const std::size_t ONE_MEGABYTE = 1048576;

	/**
	 * @struct Server
	 * @brief ...
	 */
	struct Server {
			int port;
			uint32_t ipAddress;
			unsigned long maxBodySize;
			std::string dataDirectory;
			std::vector<std::string> serverNames;
			std::map<http::StatusCode, std::string> errorPages;
			Location location;

			Server();
			Server(const Server& other);
			const Server& operator=(const Server& rhs);

			bool hasGlobalRoot() const;
			bool hasDataDir() const;

			void reset();
	};


} /* namespace config */
