#include "http/GetHandler.hpp"

#include "http/Request.hpp"

namespace http {

	GetHandler::GetHandler()
		: ARequestHandler() {
	}

	GetHandler::~GetHandler() {
	}

	void GetHandler::handle(const Request* request, Response* response) {
		if (request->getType() == Request::CGI) {
			m_cgi.handle(request, response);
			if (m_cgi.isDone()) {
				m_state = DONE;
			}
			return;
		}
	}

} /* namespace http */
