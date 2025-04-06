#pragma once

namespace io {

	enum EventResult {
		KEEP_MONITORING,
		UNREGISTER
	};

	class IEventHandler {
		public:
			virtual ~IEventHandler() {}

			virtual EventResult onReadable(int32_t fd) = 0;

			virtual EventResult onWriteable(int32_t fd) = 0;

			virtual EventResult onError(int32_t fd) = 0;
	};

} /* namespace io */
