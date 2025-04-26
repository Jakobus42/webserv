#include "core/GetRequestHandler.hpp"

#include "http/Request.hpp"

namespace core {

	GetRequestHandler::GetRequestHandler()
		: ARequestHandler() {
	}

	GetRequestHandler::~GetRequestHandler() {
	}

	bool GetRequestHandler::handle(const http::Request*, http::Response*) {
		return false;
	}

} /* namespace core */
