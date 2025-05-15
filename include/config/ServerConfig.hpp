#pragma once

#include <stdint.h>

#include "config/HttpConfig.hpp"
#include "config/LocationConfig.hpp"

#include <map>
#include <vector>

namespace config {

	struct ServerConfig {
		public:
			int port;
			uint32_t ipAddress;
			std::string socketAddress;
			std::vector<std::string> serverNames;
			LocationConfig location;
			const HttpConfig& global;

			explicit ServerConfig(const HttpConfig& globalConfig);
			~ServerConfig();
			ServerConfig(const ServerConfig& other);
			const ServerConfig& operator=(const ServerConfig& rhs);

			void validate() const;
			void print() const;

		private:
			static const std::size_t ONE_MEGABYTE = 1048576;
			static const uint32_t LOCALHOST_ADDRESS = 0x7F000001;
	};


} /* namespace config */
