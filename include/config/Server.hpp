#pragma once

#include <stdint.h>

#include "config/Location.hpp"

#include <string>
#include <vector>

namespace config {

	struct Server {
		public:
			int port;
			uint32_t ipAddress;
			unsigned long maxBodySize;
			std::string dataDirectory;
			std::vector<std::string> dataDirectoryAsTokens;
			std::vector<std::string> serverNames;
			Location location;

			Server();
			~Server();
			Server(const Server& other);
			const Server& operator=(const Server& rhs);

			bool hasRoot() const;
			bool hasDataDir() const;

			void validate() const;
			void print() const;

		private:
			static const std::size_t ONE_MEGABYTE = 1048576;
			static const uint32_t LOCALHOST_ADDRESS = 0x7F000001;
	};


} /* namespace config */
