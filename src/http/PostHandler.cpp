#include "http/PostHandler.hpp"

#include "http/Request.hpp"

namespace http {

	PostHandler::PostHandler()
		: ARequestHandler() {
	}

	PostHandler::~PostHandler() {
	}

	void PostHandler::handle(const Request*, Response*) {
		m_state = DONE;
	}


} /* namespace http */
