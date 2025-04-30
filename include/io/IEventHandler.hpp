#pragma once

#include <stdint.h>

#include "shared/NonCopyable.hpp"

namespace io {

	enum EventResult {
		KEEP_MONITORING,
		UNREGISTER
	};

	class IEventHandler : shared::mixin::NonCopyable {
		public:
			virtual ~IEventHandler() {}

			virtual EventResult onReadable(int32_t fd) = 0;

			virtual EventResult onWriteable(int32_t fd) = 0;

			virtual EventResult onHangup(int32_t fd) = 0;

			virtual EventResult onError(int32_t fd) = 0;
	};

} /* namespace io */
