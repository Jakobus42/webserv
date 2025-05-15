#include "config/HttpConfig.hpp"

#include "config/ParseException.hpp"

#include <iostream>

namespace config {

	HttpConfig::HttpConfig()
		: maxBodySize(10 * 1024 * 1024)	 // 10MB
		, maxHeaderValueLength(8 * 1024) // 8KB
		, maxHeaderCount(128)			 // 128 headers
		, maxHeaderValueCount(64)		 // 64 values
		, maxHeaderNameLength(256)		 // 256B
		, connectionTimeout(60)
		, cgiTimeout(60)
		, maxUriLength(1024)
		, dataDirectory("")
		, cgiInterpreters() {}

	HttpConfig::~HttpConfig() {}

	HttpConfig::HttpConfig(const HttpConfig& other)
		: maxBodySize(other.maxBodySize)
		, maxHeaderValueLength(other.maxHeaderValueLength)
		, maxHeaderCount(other.maxHeaderCount)
		, maxHeaderValueCount(other.maxHeaderValueCount)
		, maxHeaderNameLength(other.maxHeaderNameLength)
		, connectionTimeout(other.connectionTimeout)
		, cgiTimeout(other.cgiTimeout)
		, maxUriLength(other.maxUriLength)
		, dataDirectory(other.dataDirectory)
		, cgiInterpreters(other.cgiInterpreters) {}

	HttpConfig& HttpConfig::operator=(const HttpConfig& rhs) {
		if (this == &rhs) {
			return *this;
		}
		maxBodySize = rhs.maxBodySize;
		maxHeaderCount = rhs.maxHeaderCount;
		maxHeaderNameLength = rhs.maxHeaderNameLength;
		maxHeaderValueCount = rhs.maxHeaderValueCount;
		maxHeaderValueLength = rhs.maxHeaderValueLength;
		connectionTimeout = rhs.connectionTimeout;
		cgiTimeout = rhs.cgiTimeout;
		maxUriLength = rhs.maxUriLength;
		dataDirectory = rhs.dataDirectory;
		cgiInterpreters = rhs.cgiInterpreters;
		return *this;
	}

	void HttpConfig::print() const {
		std::cout << "-------- Http Config: --------" << std::endl;

		std::cout << "MaxBodySize: " << maxBodySize << std::endl;
		std::cout << "MaxHeaderCount: " << maxHeaderCount << std::endl;
		std::cout << "MaxHeaderNameLength: " << maxHeaderNameLength << std::endl;
		std::cout << "MaxHeaderValueCount: " << maxHeaderValueCount << std::endl;
		std::cout << "MaxHeadeaValueLength: " << maxHeaderValueLength << std::endl;
		std::cout << "ConnectionTimeout: " << connectionTimeout << std::endl;
		std::cout << "cgiTimeout: " << cgiTimeout << std::endl;
		std::cout << "maxUriLength: " << maxUriLength << std::endl;
		std::cout << "dataDirectory: " << dataDirectory << std::endl;
		for (std::map<std::string, std::string>::const_iterator interpreter = cgiInterpreters.begin(); interpreter != cgiInterpreters.end(); ++interpreter) {
			std::cout << interpreter->first << " => " << interpreter->second << std::endl;
		}

		std::cout << "--------------------------------" << std::endl;
	}

	void HttpConfig::validate() const {
		if (dataDirectory.empty()) {
			throw ParseException("HTTP block requires a 'data_dir'");
		}
	}

} /* namespace config */
