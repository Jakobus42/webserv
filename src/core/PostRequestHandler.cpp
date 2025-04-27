#include "core/PostRequestHandler.hpp"

#include "http/Request.hpp"

namespace core {

	PostRequestHandler::PostRequestHandler()
		: ARequestHandler() {
	}

	PostRequestHandler::~PostRequestHandler() {
	}

	bool PostRequestHandler::handle(const http::Request&, http::Response&) {
		return false;
	}


} /* namespace core */
