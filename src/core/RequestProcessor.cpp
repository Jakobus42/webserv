#include "core/RequestProcessor.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"

namespace core {

	RequestProcessor::RequestProcessor()
		: m_response(NULL) {}

	RequestProcessor::~RequestProcessor() { delete m_response; }

	bool RequestProcessor::processRequest(http::Request*) {
		if (!m_response) {
			m_response = new http::Response();
		}
		return false;
	}

	http::Response* RequestProcessor::releaseResponse() {
		http::Response* released = m_response;
		m_response = NULL;
		return released;
	}

	void RequestProcessor::reset() {
		delete m_response;
		m_response = NULL;
	}

} /* namespace core */
