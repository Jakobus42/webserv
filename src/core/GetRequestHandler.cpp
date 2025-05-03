#include "core/GetRequestHandler.hpp"

#include <dirent.h>

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/fileUtils.hpp"

namespace core {

	GetRequestHandler::GetRequestHandler()
		: ARequestHandler()
		, m_buffer(BUFFER_SIZE)
		, m_fileStream()
		, m_streamPosition(0) {}

	GetRequestHandler::~GetRequestHandler() {}

	// todo: make sure path exists, file exists, can access, etc. etc.
	// F_OK, R_OK, probably?
	// throw correct status code if any issues arise
	// case one: we get a directory with trailing '/'
	// case two: we get a directory with no trailing '/'
	// case three: we get an actual file with no trailing '/'
	void GetRequestHandler::checkPathPermissions(const http::Request&) const throw(http::HttpException) {
		shared::file::FileType fileType = shared::file::getFileType(m_route.absoluteFilePath);

		if (fileType == shared::file::NOT_FOUND) {
			throw http::HttpException(http::NOT_FOUND, "GET: File " + m_route.absoluteFilePath + " doesn't exist");
		}
		const config::LocationConfig& location = *m_route.location;
		if (fileType == shared::file::DIRECTORY) {
			// if location.indexFile.notEmpty() get index
			// if location.indexFile.empty() and autoindex == true generate autoindex
			// otherwise throw FORBIDDEN
			if (location.indexFile.empty() && location.autoindex == false) {
				throw http::HttpException(http::FORBIDDEN, "GET: Requested location does not have an index");
			}
		}
	}

	std::string GetRequestHandler::generateDirectoryListing(const http::Request& request, const std::string& filePath) {
		static const char* DL_TEMPLATE_END = "</ul></div></body></html>";
		static const char* DL_TEMPLATE = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
										 "content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title><style>body{font-family:Arial,sans-serif;"
										 "margin:0;padding:0;background-color:#f8f9fa;}.container{max-width:800px;margin:50px auto;padding:20px;background:#fff;box-shadow:0 "
										 "4px 6px rgba(0,0,0,0.1);border-radius:8px;}h1{font-size:1.8rem;color:#333;text-align:center;}ul{list-style-type:none;padding:0;}"
										 "li{padding:8px 10px;border-bottom:1px solid #e0e0e0;}li a{text-decoration:none;color:#007bff;transition:color 0.3s;}"
										 "li a:hover{color:#0056b3;}</style></head><body><div class=\"container\"><h1>Directory Listing</h1><ul>";
		std::string bodyTemp = DL_TEMPLATE;

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
			if (fileName == ".." && (filePath == "/" || filePath == "")) {
				continue;
			}
			std::string baseUrl = request.getUri().getAuthority();
			if (uri.getPath().empty()) {
				baseUrl += "/";
			} else {
				// Ensure uri.path starts with '/'.
				if (uri.getPath()[0] != '/')
					baseUrl += "/" + uri.getPath();
				else
					baseUrl += uri.getPath();
				// Ensure it ends with a single '/'
				if (baseUrl[baseUrl.size() - 1] != '/')
					baseUrl += "/";
			}

			std::string link;
			if (ent->d_type == DT_DIR) {
				// Add trailing slash to show it’s a directory.
				link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "/</a>";
			} else {
				link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "</a>";
			}
			// does '///' get normalized to '/' ? I think this still breaks things, so foo//bar isn't foo/bar
			bodyTemp += "<li>" + link + "</li>";
		}
		closedir(dir);
		return bodyTemp + DL_TEMPLATE_END;
	}

	void GetRequestHandler::generateAutoindexResponse(const http::Request& request, http::Response& response) {
		response.appendBody(generateDirectoryListing(request, m_route.absoluteFilePath));
		response.setStatusCode(http::OK);
	}

	void GetRequestHandler::openFile() {
		m_fileStream.open(m_route.absoluteFilePath.c_str());
		if (!m_fileStream.is_open()) {
			throw http::HttpException(http::FORBIDDEN, "GET: File " + m_route.absoluteFilePath + " could not be opened");
		}
		m_fileStream.seekg(m_streamPosition);
	}

	bool GetRequestHandler::readFile(http::Response& response) {
		m_buffer.clear();
		m_fileStream.read(m_buffer.data(), BUFFER_SIZE);
		std::streamsize bytesRead = m_fileStream.gcount();
		m_fileStream.close();
		if (bytesRead > 0) {
			response.appendBody(shared::string::StringView(m_buffer.data()));
		} else {
			m_state = DONE;
			return false;
		}
		return true;
	}

	// if file is a directory, and autoindex is true, and no index files exist
	bool GetRequestHandler::shouldAutoindex() const {
		if (!shared::file::isDirectory(m_route.absoluteFilePath)) {
			return false;
		}
		if (m_route.location->autoindex == false) {
			return false;
		}
		const std::vector<std::string>& indexFiles = m_route.location->indexFile;
		if (indexFiles.empty()) {
			return true;
		}
		// if none of the index files exist, also return true
		// todo: what if they just can't be read? forbidden? or autoindex?
		// todo: probably just do forbidden
		return true;
	}

	bool GetRequestHandler::handle(const http::Request& request, http::Response& response) throw(http::HttpException) {
		switch (m_state) {
			case PREPROCESS: {
				checkPathPermissions(request);
				if (shouldAutoindex()) {
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

	void GetRequestHandler::reset() {
		this->ARequestHandler::reset();
		m_buffer.clear();
		if (m_fileStream.is_open()) {
			m_fileStream.close();
		}
		m_fileStream.clear();
		m_streamPosition = 0;
	}

} /* namespace core */
