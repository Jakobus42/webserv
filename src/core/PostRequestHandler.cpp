#include "core/PostRequestHandler.hpp"
#include "http/Request.hpp"

namespace core {

	PostRequestHandler::PostRequestHandler()
		: ARequestHandler() {
	}

	PostRequestHandler::~PostRequestHandler() {
	}

	void PostRequestHandler::handle(const http::Request*, http::Response*) {
		m_state = DONE;
	}


} /* namespace core */
