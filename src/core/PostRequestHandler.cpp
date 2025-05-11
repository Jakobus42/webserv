#include "core/PostRequestHandler.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/fileUtils.hpp"
#include "shared/stringUtils.hpp"

namespace core {

	const std::size_t PostRequestHandler::CHUNK_SIZE = 16384; // 16 KB

	PostRequestHandler::PostRequestHandler()
		: ARequestHandler()
		, m_fileStream()
		, m_filePath("")
		, m_bytesWritten(0) {
	}

	PostRequestHandler::~PostRequestHandler() {
		if (m_fileStream.is_open()) {
			m_fileStream.close();
		}
	}

	void PostRequestHandler::reset() {
		this->ARequestHandler::reset();
		if (m_fileStream.is_open()) {
			m_fileStream.close();
		}
		m_fileStream.clear();
		m_filePath.clear();
		m_bytesWritten = 0;
	}

	std::string PostRequestHandler::generateRandomFileName() {
		static const std::size_t MAX_EXPECTED_UPLOADS_PER_SECOND = 4096;
		static std::size_t fileNo = 0;
		std::string fileName;

		fileName = shared::string::toString(static_cast<int>(time(NULL))) + "_" + shared::string::toString(fileNo) + "_upload.dat";
		fileNo += 1;
		if (fileNo >= MAX_EXPECTED_UPLOADS_PER_SECOND) {
			fileNo = 0;
		}
		return fileName;
	}

	void PostRequestHandler::checkPathPermissions(const http::Request&) const throw(http::HttpException) {
		shared::file::FileType fileType = shared::file::getFileType(m_route.absoluteFilePath);

		if (fileType == shared::file::NOT_FOUND) {
			throw http::HttpException(http::FORBIDDEN, "POST: Directory could not be found");
		}
		if (fileType == shared::file::FILE) {
			throw http::HttpException(http::FORBIDDEN, "POST: Expected directory; received file");
		}
		if (m_route.location->acceptsFileUpload() == false) {
			throw http::HttpException(http::FORBIDDEN, "POST: Location does not accept file uploads");
		}
	}

	void PostRequestHandler::createFile(const http::Request& request) throw(http::HttpException) {
		std::string filePath;

		if (request.hasHeader("x-filename")) {
			const std::vector<std::string>& filenameHeader = request.getHeader("x-filename");
			if (filenameHeader.empty() == false && filenameHeader[0].empty() == false) {
				filePath = filenameHeader[0];
			}
		}
		if (filePath.empty()) {
			filePath = generateRandomFileName();
		}
		if (filePath[0] != '/') {
			filePath = "/" + filePath;
		}
		m_filePath = m_route.absoluteFilePath + filePath;
		if (shared::file::exists(m_filePath)) {
			throw http::HttpException(http::CONFLICT, "POST: A file with this name already exists");
		}
	}

	void PostRequestHandler::openFile() throw(http::HttpException) {
		m_fileStream.open(m_filePath.c_str(), std::ios_base::app);
		if (!m_fileStream.is_open()) {
			throw http::HttpException(http::FORBIDDEN, "POST: File " + m_filePath + " could not be opened");
		}
	}

	void PostRequestHandler::writeFile(const std::string& requestBody) {
		std::size_t bytesToWrite = std::min(requestBody.size() - m_bytesWritten, CHUNK_SIZE);

		m_fileStream.write(requestBody.data() + m_bytesWritten, bytesToWrite);
		if (m_fileStream.fail()) {
			m_fileStream.close();
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "POST: Failure during write() occurred");
		}
		m_fileStream.close();
		m_bytesWritten += bytesToWrite;
	}

	bool PostRequestHandler::handle(const http::Request& request, http::Response& response) throw(http::HttpException) {
		switch (m_state) {
			case PREPROCESS: {
				checkPathPermissions(request);
				createFile(request);
				m_state = PROCESS;
				return true;
			}
			case PROCESS: {
				const std::string& requestBody = request.getBody();
				openFile();
				writeFile(requestBody);
				if (m_bytesWritten == requestBody.size()) {
					m_state = DONE;
					response.setStatusCode(http::CREATED);
				}
				return true;
			}
			case DONE:
				return false;
		}
		return false;
	}


} /* namespace core */
