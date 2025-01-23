#include "core/Dispatcher.hpp"

#include <errno.h>
#include <unistd.h>

#include "shared/Logger.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace core {

	/**
	 * @brief Constructs a new Dispatcher object.
	 */
	Dispatcher::Dispatcher()
		: m_epoll_master_fd(-1)
		, m_handlers() {
		m_epoll_master_fd = epoll_create1(0);
		if (m_epoll_master_fd < 0) {
			throw std::runtime_error("epoll_create1() failed: " + std::string(strerror(errno)));
		}
	}

	/**
	 * @brief Destroys the Dispatcher object.
	 */
	Dispatcher::~Dispatcher() {
		for (std::map<int32_t, AHandler*>::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		close(m_epoll_master_fd);
	}

	// takes ownership of the AHandler.
	void Dispatcher::registerHandler(int32_t fd, uint32_t events, AHandler* handler) {
		epoll_event event;

		event.events = events;
		event.data.fd = fd;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			delete handler;
			throw std::runtime_error("epoll_ctl() failed: " + std::string(strerror(errno)));
		}
		m_handlers.insert(std::make_pair(fd, handler));
	}

	void Dispatcher::unregisterHandler(int32_t fd) {
		epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL);
		delete m_handlers.at(fd);
		m_handlers.erase(fd);
	}

	void Dispatcher::dispatch() {
		int32_t nEvents = epoll_wait(m_epoll_master_fd, m_events, MAX_EVENTS, 60); // todo timeout
		if (nEvents < 0) {
			if (errno == EINTR) {
				return;
			}
			throw std::runtime_error("epoll_wait() failed: " + std::string(strerror(errno)));
		}

		for (int32_t i = 0; i < nEvents; ++i) {
			const epoll_event& event = m_events[i];
			AHandler* handler = m_handlers.at(event.data.fd);

			handler->handle(event.data.fd, event.events);
			if (handler->isDone()) {
				this->unregisterHandler(event.data.fd);
			}
		}
	}

} /* namespace core */
