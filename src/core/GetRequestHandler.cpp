#include "core/GetRequestHandler.hpp"

#include <dirent.h>

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/fileUtils.hpp"

#include <cstring>

namespace core {

	static const char* DIRECTORY_LISTING_TEMPLATE = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
													"content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title><style>body{font-family:Arial,sans-serif;"
													"margin:0;padding:0;background-color:#f8f9fa;}.container{max-width:800px;margin:50px auto;padding:20px;background:#fff;box-shadow:0 "
													"4px 6px rgba(0,0,0,0.1);border-radius:8px;}h1{font-size:1.8rem;color:#333;text-align:center;}ul{list-style-type:none;padding:0;}"
													"li{padding:8px 10px;border-bottom:1px solid #e0e0e0;}li a{text-decoration:none;color:#007bff;transition:color 0.3s;}"
													"li a:hover{color:#0056b3;}</style></head><body><div class=\"container\"><h1>Directory Listing</h1><ul>";
	static const char* DIRECTORY_LISTING_TEMPLATE_END = "</ul></div></body></html>";

	GetRequestHandler::GetRequestHandler()
		: ARequestHandler()
		, m_buffer(BUFFER_SIZE)
		, m_fileStream()
		, m_streamPosition(0)
		, m_fileType(shared::file::FILE)
		, m_filePath("")
		, m_shouldAutoindex(false) {}

	GetRequestHandler::~GetRequestHandler() {}

	void GetRequestHandler::reset() {
		this->ARequestHandler::reset();
		std::memset(m_buffer.data(), '\0', m_buffer.size());
		if (m_fileStream.is_open()) {
			m_fileStream.close();
		}
		m_fileStream.clear();
		m_fileType = shared::file::FILE;
		m_filePath.clear();
		m_shouldAutoindex = false;
		m_streamPosition = 0;
	}

	// todo: make sure path exists, file exists, can access, etc. etc.
	// F_OK, R_OK, probably?
	// throw correct status code if any issues arise
	// case one: we get a directory with trailing '/'
	// case two: we get a directory with no trailing '/'
	// case three: we get an actual file with no trailing '/'
	void GetRequestHandler::checkPathPermissions(const http::Request&) const throw(http::HttpException) {
		if (m_fileType == shared::file::NOT_FOUND) {
			throw http::HttpException(http::NOT_FOUND, "GET: File " + m_route.absoluteFilePath + " doesn't exist");
		}
		const config::LocationConfig& location = *m_route.location;
		if (m_fileType == shared::file::DIRECTORY) {
			if (location.indexFile.empty() && location.autoindex == false) {
				throw http::HttpException(http::FORBIDDEN, "GET: Requested directory does not have an index");
			}
		}
	}

	std::string GetRequestHandler::generateDirectoryListing(const http::Request& request, const std::string& filePath) {
		std::string bodyTemp = DIRECTORY_LISTING_TEMPLATE;

		DIR* dir;
		if ((dir = opendir(filePath.c_str())) == NULL) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "GET: getDirectoryListing: Couldn't open directory");
		}
		struct dirent* ent;
		const http::Uri& uri = request.getUri();
		while ((ent = readdir(dir)) != NULL) {
			std::string fileName(ent->d_name);

			if (fileName == ".") {
				continue;
			}
			if (fileName == ".." && m_route.location->isServerRoot == true && m_route.remainingPath == "/") {
				continue;
			}
			std::string baseUrl = request.getUri().getAuthority();
			const std::string& uriPath = uri.getPath();
			if (uriPath.empty()) {
				baseUrl += "/";
			} else {
				if (uriPath[0] != '/')
					baseUrl += "/" + uriPath;
				else
					baseUrl += uriPath;
				if (baseUrl[baseUrl.size() - 1] != '/')
					baseUrl += "/";
			}

			std::string link;
			if (ent->d_type == DT_DIR) {
				link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "/</a>";
			} else {
				link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "</a>";
			}
			// does '///' get normalized to '/' ? I think this still breaks things, so foo//bar isn't foo/bar
			bodyTemp += "<li>" + link + "</li>";
		}
		closedir(dir);
		return bodyTemp + DIRECTORY_LISTING_TEMPLATE_END;
	}

	void GetRequestHandler::generateAutoindexResponse(const http::Request& request, http::Response& response) {
		response.appendBody(generateDirectoryListing(request, m_route.absoluteFilePath));
		response.setStatusCode(http::OK);
	}

	void GetRequestHandler::selectFile() {
		if (m_fileType == shared::file::FILE) {
			m_filePath = m_route.absoluteFilePath;
		} else {
			const config::LocationConfig& location = *m_route.location;
			for (std::vector<std::string>::const_iterator indexFile = location.indexFile.begin(); indexFile != location.indexFile.end(); ++indexFile) {
				m_filePath = m_route.absoluteFilePath + "/" + *indexFile;
				if (shared::file::isRegularFile(m_filePath) == true) {
					return;
				}
			}
			if (location.autoindex == true) {
				m_shouldAutoindex = true;
			} else {
				throw http::HttpException(http::FORBIDDEN, "GET: Index for location " + location.path + " not found or forbidden");
			}
		}
	}

	void GetRequestHandler::openFile() {
		m_fileStream.open(m_filePath.data());
		if (!m_fileStream.is_open()) {
			throw http::HttpException(http::FORBIDDEN, "GET: File " + m_route.absoluteFilePath + " could not be opened");
		}
		m_fileStream.seekg(m_streamPosition);
	}

	bool GetRequestHandler::readFile(http::Response& response) {
		m_fileStream.read(m_buffer.data(), BUFFER_SIZE);
		if (m_fileStream.bad()) {
			m_fileStream.close();
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "GET: Failure during read() occurred");
		}
		std::streamsize bytesRead = m_fileStream.gcount();
		m_fileStream.close();
		m_streamPosition += bytesRead;
		if (bytesRead > 0) {
			response.appendBody(shared::string::StringView(m_buffer.data(), bytesRead));
			std::memset(m_buffer.data(), '\0', m_buffer.size());
		} else {
			m_state = DONE;
			return false;
		}
		return true;
	}

	bool GetRequestHandler::handle(const http::Request& request, http::Response& response) throw(http::HttpException) {
		switch (m_state) {
			case PREPROCESS: {
				m_fileType = shared::file::getFileType(m_route.absoluteFilePath);
				checkPathPermissions(request);
				selectFile();
				if (m_shouldAutoindex == true) {
					generateAutoindexResponse(request, response);
					m_state = DONE;
				} else {
					m_state = PROCESS;
				}
				return true;
			}
			case PROCESS: {
				openFile();
				readFile(response);
				return true;
			}
			case DONE: {
				return false;
			}
		}
		return false;
	}

} /* namespace core */
