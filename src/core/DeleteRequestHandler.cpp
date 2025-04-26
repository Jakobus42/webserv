#include "core/DeleteRequestHandler.hpp"

namespace core {

	DeleteRequestHandler::DeleteRequestHandler()
		: ARequestHandler() {
	}

	DeleteRequestHandler::~DeleteRequestHandler() {
	}

	bool DeleteRequestHandler::handle(const http::Request*, http::Response*) {
		return false;
	}

} /* namespace core */
