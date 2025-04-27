#include "core/DeleteRequestHandler.hpp"

namespace core {

	DeleteRequestHandler::DeleteRequestHandler()
		: ARequestHandler() {
	}

	DeleteRequestHandler::~DeleteRequestHandler() {
	}

	void DeleteRequestHandler::handle(const http::Request&, http::Response&) {
		m_state = DONE;
	}

} /* namespace core */
