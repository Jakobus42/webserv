#include "core/GetRequestHandler.hpp"

#include "http/Request.hpp"

namespace core {

	GetRequestHandler::GetRequestHandler()
		: ARequestHandler() {
	}

	GetRequestHandler::~GetRequestHandler() {
	}

	void GetRequestHandler::handle(const http::Request*, http::Response*) {
		m_state = DONE;
	}

} /* namespace core */
