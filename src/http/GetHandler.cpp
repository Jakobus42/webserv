#include "http/GetHandler.hpp"

#include "http/Request.hpp"

namespace http {

	GetHandler::GetHandler()
		: ARequestHandler() {
	}

	GetHandler::~GetHandler() {
	}

	void GetHandler::handle(const Request*, Response*) {
		m_state = DONE;
	}

} /* namespace http */
