#pragma once
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>

namespace shared {

	namespace file {

		bool exists(const std::string& path);
		bool fileExists(const std::string& path);
		bool isRegularFile(const std::string& path);
		bool isDirectory(const std::string& path);
		bool isReadable(const std::string& path);
		bool isWritable(const std::string& path);
		bool isExecutable(const std::string& path);
		bool directoryExists(const std::string& path);
		bool dirIsAccessible(const std::string& path);

	} // namespace file

} // namespace shared
