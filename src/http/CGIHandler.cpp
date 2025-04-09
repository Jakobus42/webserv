#include "http/CGIHandler.hpp"

namespace http {

	CGIHandler::CGIHandler() {
	}

	CGIHandler::~CGIHandler() {
	}

	void CGIHandler::handle(const Request* request, Response* response) {
		switch (m_state) {
			case PREPROCESS:
			case PROCESS:
			case DONE: break;
		}
	}

} /* namespace http */
