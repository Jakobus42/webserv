#include "shared/fileUtils.hpp"

namespace shared {

	namespace file {

		bool exists(const std::string& path) {
			struct stat info;
			return (stat(path.c_str(), &info) == 0);
		}

		bool fileExists(const std::string& path) {
			return exists(path);
		}

		bool isRegularFile(const std::string& path) {
			struct stat info;
			if (stat(path.c_str(), &info) != 0)
				return false;
			return S_ISREG(info.st_mode);
		}

		bool isDirectory(const std::string& path) {
			struct stat info;
			if (stat(path.c_str(), &info) != 0) {
				return false;
			}
			return S_ISDIR(info.st_mode);
		}

		// Check if a file is readable
		bool isReadable(const std::string& path) {
			struct stat info;
			if (stat(path.c_str(), &info) != 0) {
				return false;
			}
			return (access(path.c_str(), R_OK) == 0 && (info.st_mode & S_IRUSR) != 0);
		}

		// Check if a file is writable
		bool isWritable(const std::string& path) {
			struct stat info;
			if (stat(path.c_str(), &info) != 0) {
				return false;
			}
			return (access(path.c_str(), W_OK) == 0);
		}

		bool isDirUsable(const std::string& path) {
			struct stat info;
			if (stat(path.c_str(), &info) != 0) {
				return false;
			}
			// Check if it's a directory and has read+write+execute permissions
			return S_ISDIR(info.st_mode) && (info.st_mode & S_IRUSR) && (info.st_mode & S_IWUSR) && (info.st_mode & S_IXUSR);
		}

		// Check if a file is executable
		bool isExecutable(const std::string& path) {
			return (access(path.c_str(), X_OK) == 0);
		}

		// Check if a directory exists
		bool directoryExists(const std::string& path) {
			struct stat info;
			return (stat(path.c_str(), &info) == 0) && (info.st_mode & S_IFDIR);
		}

		// Check if a directory is accessible (read & execute)
		bool dirIsAccessible(const std::string& path) {
			return access(path.c_str(), R_OK | X_OK) == 0;
		}

	} // namespace file

} // namespace shared
