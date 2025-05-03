#include "core/PostRequestHandler.hpp"

#include "http/Request.hpp"

namespace core {

	PostRequestHandler::PostRequestHandler()
		: ARequestHandler()
		, m_fileStream() {
	}

	PostRequestHandler::~PostRequestHandler() {
	}

	void PostRequestHandler::checkPathPermissions() const throw(http::HttpException) {
	}

	void PostRequestHandler::openFile() const throw(http::HttpException) {
	}

	void PostRequestHandler::writeFile() {
	}

	bool PostRequestHandler::handle(const http::Request&, http::Response&) {
		switch (m_state) {
			case PREPROCESS:
				checkPathPermissions();
				m_state = PROCESS;
				return true;
			case PROCESS:
				openFile();
				writeFile();
				m_state = DONE;
				return true;
			case DONE:
				return false;
		}
		return false;
	}


} /* namespace core */
