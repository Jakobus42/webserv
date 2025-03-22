#include "http/GetHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new GetHandler object.
	 */
	GetHandler::GetHandler(Router& router)
		: ARequestHandler(router)
		, m_fileStream() {}

	/**
	 * @brief Destroys the GetHandler object.
	 */
	GetHandler::~GetHandler() {}

	void GetHandler::getFilePath(const config::Location& location, const std::string& filePath, std::string& target) {
		for (std::vector<std::string>::const_iterator fileName = location.indexFile.begin(); fileName != location.indexFile.end(); ++fileName) {
			std::string tmpFilePath = filePath + "/" + *fileName;
			FileType tmpFileType = m_router.checkFileType(tmpFilePath);
			if (tmpFileType == FILE) { // return path to first file in index that exists and isn't a directory
				std::cout << "getFilePath found file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
				target.assign(tmpFilePath);
				return;
			}
		}
	}

	void GetHandler::openFile(const Request& request, Response& response) {
		const config::Location& location = *request.getLocation();
		FileType fileType = request.getFileType();

		if (fileType == _NOT_FOUND) {
			throw http::exception(NOT_FOUND, "GET: File doesn't exist");
		}
		if (fileType == DIRECTORY) {
			if (!location.indexFile.empty()) {
				getFilePath(location, request.getUri().safeAbsolutePath, m_filePath); // returns true only if filePath isn't still ""
			}
			if (m_filePath.empty()) { // we didn't find a file or config doesn't have index file
				if (location.autoindex == true) {
					std::string autoindexBody = getDirectoryListing(request.getUri(), location);
					response.setBody(autoindexBody);
					response.setHeader("Content-Length", shared::string::to_string(autoindexBody.size()));
					response.setStatusCode(OK);
					m_state = DONE;
					return; // no more work to be done, return true
				}
				throw http::exception(FORBIDDEN, "GET: Requested location does not have an index");
			}
		}
		if (fileType == FILE) {
			m_filePath = request.getUri().safeAbsolutePath;
		}


		m_fileStream.open(m_filePath.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!m_fileStream.is_open()) {
			throw http::exception(FORBIDDEN, "GET: File could not be opened"); // TODO: also happens if path doesn't exist, should be NOT_FOUND in that case
		}

		m_state = PROCESSING;
	}

	void GetHandler::readFile(Response& response) {
		std::vector<char> buffer(this->getBufferSize());

		m_fileStream.read(buffer.data(), this->getBufferSize());
		std::streamsize bytesRead = m_fileStream.gcount();

		if (bytesRead > 0) {
			response.appendToBody(buffer.data(), bytesRead);
			return;
		}

		m_fileStream.close();
		response.setHeader("Content-Length", shared::string::to_string(response.getBody().size()));
		response.setStatusCode(OK);
		m_state = DONE;
	}

	// in GET, there should be a file name in the path
	// this whole thing is mad sus, TODO: address
	void GetHandler::handle(const Request& request, Response& response) {
		switch (m_state) {
			case PENDING: {
				return openFile(request, response);
			}

			case PROCESSING: {
				return readFile(response);
			}

			case DONE:
				return;

			default:
				throw http::exception(INTERNAL_SERVER_ERROR, "GetHandler: Invalid handler state");
		}
	}

	void GetHandler::reset() {
		this->ARequestHandler::reset();
		m_fileStream.close();
		m_fileStream.clear();
	}

} /* namespace http */
