#include "core/CGIProcessor.hpp"

#include "http/Response.hpp"

namespace core {

	CGIProcessor::CGIProcessor(io::Dispatcher& dispatcher)
		: m_dispatcher(dispatcher) {
		(void)m_dispatcher;
	}

	CGIProcessor::~CGIProcessor() {}

	bool CGIProcessor::process(http::Request&) { return false; }

	http::Response* CGIProcessor::releaseResponse() {
		return new http::Response();
	}

	void CGIProcessor::reset() {}

} /* namespace core */
