#include "http/PostHandler.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "shared/fileUtils.hpp"

namespace http {

	/**
	 * @brief Constructs a new PostHandler object.
	 */
	PostHandler::PostHandler(Router& router)
		: ARequestHandler(router)
		, m_fileStream()
		, m_bytesWritten(0) {}

	/**
	 * @brief Destroys the PostHandler object.
	 */
	PostHandler::~PostHandler() {}

	std::string PostHandler::generateFileName() const {
		static const std::size_t MAX_UPLOADS_PER_SECOND = 4096;
		static std::size_t lol = 0;
		std::string fileName;

		fileName = shared::string::fromNum(static_cast<int>(time(NULL))) + "_" + shared::string::fromNum(lol) + "_upload.dat";
		lol += 1;
		if (lol >= MAX_UPLOADS_PER_SECOND) {
			lol = 0;
		}
		return fileName;
	}

	void PostHandler::createFile(const Request& request) {
		FileType fileType = request.getFileType();

		if (fileType == _NOT_FOUND) {
			throw http::exception(FORBIDDEN, "POST: Directory could not be found");
		}
		if (fileType == FILE) {
			throw http::exception(FORBIDDEN, "POST: Expected directory; received file");
		}
		if (!request.getLocation()->acceptsFileUpload()) {
			throw http::exception(FORBIDDEN, "Location does not accept file uploads (no POST)");
		}

		std::string fileName = generateFileName();

		if (request.hasHeader("x-cool-filename")) {
			const std::vector<std::string>& filenameHeader = request.getHeader("x-cool-filename");
			if (!filenameHeader.empty() && !filenameHeader.at(0).empty()) {
				fileName = filenameHeader.at(0);
			}
		}

		std::string absoluteFilePath = request.getUri().safeAbsolutePath + "/" + fileName;

		if (shared::file::fileExists(absoluteFilePath)) {
			throw http::exception(CONFLICT, "POST: A file with this name already exists: " + fileName);
		}
		m_fileStream.open(absoluteFilePath.c_str(), std::ios::binary);
		m_state = PROCESSING;
	}

	// in POST, there should be no file name in the path
	// TODO: currently returns 403 forbidden if the file
	// in the path exists and 404 not found if it doesn't
	void PostHandler::handle(const Request& request, Response& response) {
		switch (m_state) {
			case PENDING: {
				return createFile(request);
			}

			case PROCESSING: {
				const std::string& requestBody = request.getBody();

				try {
					size_t bytesToWrite = std::min(requestBody.size() - m_bytesWritten, getChunkSize());

					m_fileStream.write(requestBody.data() + m_bytesWritten, bytesToWrite); // TODO: verify whether this could break & if so, what error to throw
					m_bytesWritten += bytesToWrite;
				} catch (std::exception& e) {
					std::cerr << "PostHandler::handle(): " << e.what() << std::endl;
					m_fileStream.close();
					throw http::exception(INTERNAL_SERVER_ERROR, "PostHandler: Failure during write() occurred");
				}
				if (m_bytesWritten == requestBody.size()) {
					m_fileStream.close();
					response.setStatusCode(CREATED);
					response.setHeader("Content-Length", "0");
					m_state = DONE;
				}
			}

			case DONE:
				return;

			default:
				throw http::exception(INTERNAL_SERVER_ERROR, "PostHandler: Invalid Handler State");
		}
	}

	void PostHandler::reset() {
		this->ARequestHandler::reset();
		m_fileStream.close();
		m_fileStream.clear();
		m_bytesWritten = 0;
	}

} /* namespace http */
