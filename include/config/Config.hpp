#pragma once
#include "config/HttpConfig.hpp"
#include "config/ServerConfig.hpp"

#include <map>
#include <vector>

namespace config {

	class Config {
		public:
			typedef std::vector<ServerConfig> ServerConfigs;
			typedef std::map<std::string /*address*/, ServerConfigs> ListenServerConfigs;

			HttpConfig globalConfig;
			ListenServerConfigs listenServerConfigs;
			std::vector<ServerConfig> serverConfigs;

		private:
	};

} /* namespace config */
