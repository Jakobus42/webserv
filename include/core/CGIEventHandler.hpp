#pragma once

#include "io/IEventHandler.hpp"

namespace core {

	class CGIEventHandler : public io::IEventHandler {
		public:
			CGIEventHandler();
			~CGIEventHandler();

			io::EventResult onReadable(int32_t fd);

			io::EventResult onWriteable(int32_t fd);

			io::EventResult onError(int32_t fd);

		private:
	};

} /* namespace core */
