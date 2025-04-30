#pragma once

#include <stdint.h>

#include "config/LocationConfig.hpp"

#include <vector>

namespace config {

	struct ServerConfig {
		public:
			int port;
			uint32_t ipAddress;
			unsigned long maxBodySize;
			unsigned long maxHeaderValueLength;
			unsigned long maxHeaderCount;
			unsigned long maxHeaderValueCount;
			unsigned long maxHeaderNameLength;
			std::string dataDirectory;
			std::vector<std::string> serverNames;
			LocationConfig location;

			ServerConfig();
			~ServerConfig();
			ServerConfig(const ServerConfig& other);
			const ServerConfig& operator=(const ServerConfig& rhs);

			bool hasRoot() const;
			bool hasDataDir() const;

			void validate() const;
			void print() const;

		private:
			static const std::size_t ONE_MEGABYTE = 1048576;
			static const uint32_t LOCALHOST_ADDRESS = 0x7F000001;
	};


} /* namespace config */
