#include "core/DeleteRequestHandler.hpp"

#include "http/Response.hpp"
#include "shared/fileUtils.hpp"

#include <cstdio>

namespace core {

	DeleteRequestHandler::DeleteRequestHandler()
		: ARequestHandler() {}

	DeleteRequestHandler::~DeleteRequestHandler() {}

	// todo: ensure these codes are correct
	// todo: maybe check for F_OK and W_OK, maybe more?
	void DeleteRequestHandler::checkPathPermissions(const http::Request&) const throw(http::HttpException) {
		shared::file::FileType fileType = shared::file::getFileType(m_route.filePath);

		if (fileType == shared::file::NOT_FOUND) {
			throw http::HttpException(http::NOT_FOUND, "DELETE: File doesn't exist");
		}
		if (fileType == shared::file::DIRECTORY) {
			throw http::HttpException(http::FORBIDDEN, "DELETE: Expected file; received directory");
		}
	}

	void DeleteRequestHandler::deleteFile() throw(http::HttpException) {
		if (std::remove(m_route.filePath.c_str()) != 0) {
			throw http::HttpException(http::FORBIDDEN, "DELETE: File could not be removed");
		}
	}

	bool DeleteRequestHandler::handle(const http::Request& request, http::Response& response) throw(http::HttpException) {
		switch (m_state) {
			case PREPROCESS:
				checkPathPermissions(request);
				m_state = PROCESS;
				return true;
			case PROCESS: {
				deleteFile();
				response.setStatusCode(http::NO_CONTENT);
				m_state = DONE;
				return true;
			}
			case DONE:
				return false;
		}
		return false;
	}

} /* namespace core */
