#pragma once

#include "core/VirtualServer.hpp"
#include "io/Dispatcher.hpp"
#include "io/IEventHandler.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class AcceptHandler : public io::IEventHandler {
		public:
			AcceptHandler(core::VirtualServer* vServer, io::Dispatcher& dispatcher);
			virtual ~AcceptHandler();

			virtual io::EventResult onReadable(int32_t fd);

			virtual io::EventResult onWriteable(int32_t fd);

			virtual io::EventResult onError(int32_t fd);

		private:
			VirtualServer* m_vServer;
			io::Dispatcher& m_dispatcher;
	};

} /* namespace core */
