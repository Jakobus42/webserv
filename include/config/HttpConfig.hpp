#pragma once
#include <map>
#include <string>

namespace config {

	struct HttpConfig {
		public:
			unsigned long maxBodySize;
			unsigned long maxHeaderValueLength;
			unsigned long maxHeaderCount;
			unsigned long maxHeaderValueCount;
			unsigned long maxHeaderNameLength;
			unsigned long connectionTimeout;
			unsigned long cgiTimeout;
			unsigned long maxUriLength;
			std::map<std::string, std::string> cgiInterpreters;

			HttpConfig();
			~HttpConfig();
			HttpConfig(const HttpConfig& other);
			HttpConfig& operator=(const HttpConfig& rhs);

			void print() const;
	};

} /* namespace config */
