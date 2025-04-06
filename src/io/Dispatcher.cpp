#include "io/Dispatcher.hpp"

#include "io/IEventHandler.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

namespace io {

	Dispatcher::Dispatcher()
		: m_multiplexer()
		, m_handlers()
		, m_running(false) {
	}

	Dispatcher::~Dispatcher() {
		HandlerMap::iterator it = m_handlers.begin();
		while (it != m_handlers.end()) {
			int32_t fd = it->first;
			++it;

			try {
				unregisterHandler(fd);
			} catch (const std::exception& e) {
				LOG_ERROR("failed to unregister handler for fd " + shared::string::toString(fd) + e.what());
			}
		}
	}

	void Dispatcher::registerHandler(int32_t fd, IEventHandler* handler, uint32_t events) {
		if (!handler) {
			throw std::runtime_error("cannot register null handler");
		}

		if (m_handlers.find(fd) != m_handlers.end()) {
			throw std::runtime_error("handler already registered for this file descriptor");
		}

		m_multiplexer.add(fd, events);
		m_handlers[fd] = handler;
	}

	void Dispatcher::modifyEvents(int32_t fd, uint32_t events) {
		if (m_handlers.find(fd) == m_handlers.end()) {
			throw std::runtime_error("no handler registered for this file descriptor");
		}

		m_multiplexer.modify(fd, events);
	}

	void Dispatcher::unregisterHandler(int32_t fd) {
		HandlerMap::iterator it = m_handlers.find(fd);
		if (it == m_handlers.end()) {
			throw std::runtime_error("no handler registered for this file descriptor");
		}

		m_multiplexer.remove(fd);
		m_handlers.erase(it);
	}

	int32_t Dispatcher::dispatchOnce(int32_t timeoutMs) {
		int32_t numEvents = m_multiplexer.poll(timeoutMs);

		const AMultiplexer::Events& events = m_multiplexer.getReadyEvents();
		for (size_t i = 0; i < events.size(); ++i) {
			handleEvent(events[i]);
		}

		return numEvents;
	}

	void Dispatcher::dispatch(int32_t timeoutMs) {
		m_running = true;

		while (m_running) {
			dispatchOnce(timeoutMs);
		}
	}

	void Dispatcher::stop() {
		m_running = false;
	}

	void Dispatcher::handleEvent(const AMultiplexer::Event& event) {
		IEventHandler* handler = m_handlers.at(event.fd);
		EventResult result = KEEP_MONITORING;

		if (event.events & AMultiplexer::EVENT_ERROR) {
			result = handler->onError(event.fd);
		}

		if (result != UNREGISTER && event.events & AMultiplexer::EVENT_READ) {
			result = handler->onReadable(event.fd);
		}

		if (result != UNREGISTER && event.events & AMultiplexer::EVENT_WRITE) {
			result = handler->onWriteable(event.fd);
		}

		if (result == UNREGISTER) {
			try {
				unregisterHandler(event.fd);
			} catch (const std::exception& e) {
				LOG_ERROR("failed to unregister handler for fd " + shared::string::toString(event.fd) + e.what());
			}
		}
	}

} /* namespace io */