#include "http/PostHandler.hpp"

#include "http/Request.hpp"

namespace http {

	PostHandler::PostHandler()
		: ARequestHandler() {
	}

	PostHandler::~PostHandler() {
	}

	void PostHandler::handle(const Request* request, Response* response) {
		if (request->getType() == Request::CGI) {
			m_cgi.handle(request, response);
			if (m_cgi.isDone()) {
				m_state = DONE;
			}
			return;
		}
	}


} /* namespace http */
