#include "core/AcceptHandler.hpp"

namespace core {

	AcceptHandler::AcceptHandler() {
	}

	AcceptHandler::~AcceptHandler() {
	}

	io::EventResult AcceptHandler::onReadable(int32_t) {
		return io::KEEP_MONITORING;
	}

	io::EventResult AcceptHandler::onWriteable(int32_t) {
		return io::KEEP_MONITORING;
	}

	io::EventResult AcceptHandler::onError(int32_t) {
		return io::UNREGISTER;
	}


} /* namespace core */
