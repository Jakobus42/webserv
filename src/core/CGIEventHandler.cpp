#include "core/CGIEventHandler.hpp"

#include <iostream>

namespace core {

	CGIEventHandler::CGIEventHandler() {
	}

	CGIEventHandler::~CGIEventHandler() {
	}

	io::EventResult CGIEventHandler::onReadable(int32_t) {
		return io::KEEP_MONITORING;
	}

	io::EventResult CGIEventHandler::onWriteable(int32_t) {
		return io::KEEP_MONITORING;
	}

	io::EventResult CGIEventHandler::onError(int32_t) {
		return io::KEEP_MONITORING;
	}

} /* namespace core */
