#include "core/GetRequestHandler.hpp"

#include <dirent.h>

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/fileUtils.hpp"

#include <cstring>
#include <iomanip>

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

	const std::string& GetRequestHandler::getMimeType() const {
		static std::string octetStreamType = "application/octet-stream";
		static std::map<std::string, std::string> contentTypes;

		if (contentTypes.empty()) {
			contentTypes[".aac"] = "audio/aac";
			contentTypes[".avi"] = "video/x-msvideo";
			contentTypes[".bin"] = "application/octet-stream";
			contentTypes[".bmp"] = "image/bmp";
			contentTypes[".css"] = "text/css";
			contentTypes[".csv"] = "text/csv";
			contentTypes[".eot"] = "application/vnd.ms-fontobject";
			contentTypes[".gz"] = "application/gzip";
			contentTypes[".gif"] = "image/gif";
			contentTypes[".htm"] = "text/html";
			contentTypes[".html"] = "text/html";
			contentTypes[".ico"] = "image/vnd.microsoft.icon";
			contentTypes[".jar"] = "application/java-archive";
			contentTypes[".jpg"] = "image/jpeg";
			contentTypes[".jpeg"] = "image/jpeg";
			contentTypes[".js"] = "application/javascript";
			contentTypes[".json"] = "application/json";
			contentTypes[".jsonld"] = "application/ld+json";
			contentTypes[".mjs"] = "text/javascript";
			contentTypes[".mp3"] = "audio/mpeg";
			contentTypes[".mp4"] = "video/mp4";
			contentTypes[".mpeg"] = "video/mpeg";
			contentTypes[".oga"] = "audio/ogg";
			contentTypes[".ogg"] = "audio/opus";
			contentTypes[".ogv"] = "video/ogg";
			contentTypes[".ogx"] = "application/ogg";
			contentTypes[".opus"] = "audio/ogg";
			contentTypes[".otf"] = "font/otf";
			contentTypes[".pdf"] = "application/pdf";
			contentTypes[".php"] = "application/x-httpd-php";
			contentTypes[".png"] = "image/png";
			contentTypes[".rar"] = "application/vnd.rar";
			contentTypes[".rtf"] = "application/rtf";
			contentTypes[".sh"] = "application/x-sh";
			contentTypes[".svg"] = "image/svg+xml";
			contentTypes[".tar"] = "application/x-tar";
			contentTypes[".tif"] = "image/tiff";
			contentTypes[".tiff"] = "image/tiff";
			contentTypes[".ttf"] = "font/ttf";
			contentTypes[".txt"] = "text/plain";
			contentTypes[".wav"] = "audio/wav";
			contentTypes[".webm"] = "video/webm";
			contentTypes[".webp"] = "image/webp";
			contentTypes[".xhtml"] = "application/xhtml+xml";
			contentTypes[".xml"] = "application/xml";
			contentTypes[".zip"] = "application/zip";
			contentTypes[".7z"] = "application/x-7z-compressed";
		}

		std::size_t lastDot = m_filePath.rfind('.');
		if (lastDot == std::string::npos) {
			return octetStreamType;
		}
		std::string fileExtension = m_filePath.substr(lastDot);
		if (fileExtension.find('/') != std::string::npos) {
			return octetStreamType;
		}
		std::map<std::string, std::string>::iterator type = contentTypes.find(fileExtension);
		if (type == contentTypes.end()) {
			return octetStreamType;
		}
		return type->second;
	}

	void GetRequestHandler::checkPathPermissions(const http::Request&) const throw(http::HttpException) {
		if (m_fileType == shared::file::NOT_FOUND) {
			throw http::HttpException(http::NOT_FOUND, "GET: File " + m_route.filePath + " doesn't exist");
		}
		const config::LocationConfig& location = *m_route.location;
		if (m_fileType == shared::file::DIRECTORY) {
			if (location.indexFile.empty() && location.autoindex == false) {
				throw http::HttpException(http::FORBIDDEN, "GET: Requested directory does not have an index");
			}
		}
	}

	std::string GetRequestHandler::hexEncode(const std::string& str) {
		static const std::string UNRESERVED_CHARACTERS = "-_.~";
		std::ostringstream oss;

		for (std::size_t i = 0; i < str.size(); ++i) {
			unsigned char c = static_cast<unsigned char>(str[i]);

			if (isalnum(str[i]) || UNRESERVED_CHARACTERS.find(str[i]) != std::string::npos) {
				oss << str[i];
			} else {
				oss << '%'
					<< std::uppercase
					<< std::setw(2)
					<< std::setfill('0')
					<< std::hex
					<< static_cast<int>(c);
				oss << std::dec << std::nouppercase;
			}
		}
		return oss.str();
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
				link = "<a href=\"http://" + baseUrl + hexEncode(fileName) + "\">" + fileName + "/</a>";
			} else {
				link = "<a href=\"http://" + baseUrl + hexEncode(fileName) + "\">" + fileName + "</a>";
			}
			// does '///' get normalized to '/' ? I think this still breaks things, so foo//bar isn't foo/bar
			bodyTemp += "<li>" + link + "</li>";
		}
		closedir(dir);
		return bodyTemp + DIRECTORY_LISTING_TEMPLATE_END;
	}

	void GetRequestHandler::generateAutoindexResponse(const http::Request& request, http::Response& response) {
		response.appendBody(generateDirectoryListing(request, m_route.filePath));
		response.setStatusCode(http::OK);
		response.setHeader("Content-Type", "text/html");
	}

	void GetRequestHandler::selectFile() {
		if (m_fileType == shared::file::FILE) {
			m_filePath = m_route.filePath;
		} else {
			const config::LocationConfig& location = *m_route.location;
			for (std::vector<std::string>::const_iterator indexFile = location.indexFile.begin(); indexFile != location.indexFile.end(); ++indexFile) {
				m_filePath = m_route.filePath + "/" + *indexFile;
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
			throw http::HttpException(http::FORBIDDEN, "GET: File " + m_route.filePath + " could not be opened");
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
				m_fileType = shared::file::getFileType(m_route.filePath);
				checkPathPermissions(request);
				selectFile();
				if (m_shouldAutoindex == true) {
					generateAutoindexResponse(request, response);
					m_state = DONE;
				} else {
					response.setHeader("Content-Type", getMimeType());
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
