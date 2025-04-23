#include "http/DeleteHandler.hpp"

namespace http {

	DeleteHandler::DeleteHandler()
		: ARequestHandler() {
	}

	DeleteHandler::~DeleteHandler() {
	}

	void DeleteHandler::handle(const Request*, Response*) {
		m_state = DONE;
	}

} /* namespace http */
