#pragma once

#include "io/IEventHandler.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class AcceptHandler : public io::IEventHandler {
		public:
			AcceptHandler();
			virtual ~AcceptHandler();

			virtual io::EventResult onReadable(int32_t fd);

			virtual io::EventResult onWriteable(int32_t fd);

			virtual io::EventResult onError(int32_t fd);

		private:
	};

} /* namespace core */
