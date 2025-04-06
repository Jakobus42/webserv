#if defined(__linux__)

#	include "io/EpollMultiplexer.hpp"

#	include <cstring>
#	include <sys/epoll.h>
#	include <unistd.h>

#	include "io/AHandler.hpp"
#	include "shared/Logger.hpp"

namespace io {

	EpollMultiplexer::EpollMultiplexer()
		: AMultiplexer()
		, m_epollFd(-1) {
		m_epollFd = epoll_create1(0);
		if (m_epollFd == -1) {
			throw std::runtime_error(std::string("failed to create epoll instance: ") + strerror(errno));
		}
	}

	EpollMultiplexer::~EpollMultiplexer() {
		if (m_epollFd != -1) {
			close(m_epollFd);
		}
	}

	void EpollMultiplexer::subscribe(int32_t fd, uint32_t events, AHandler* handler) {
		if (!handler) {
			throw std::runtime_error("null handler provided");
		}

		if (fd < 0) {
			throw std::runtime_error("invalid fd");
		}

		struct epoll_event ev;
		std::memset(&ev, 0, sizeof(ev));
		ev.events = convertToEpollEvents(events);
		ev.data.fd = fd;

		HandlerMap::iterator it = m_handlers.find(fd);
		if (it != m_handlers.end()) {
			unsubscribe(fd);
		}

		if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
			throw std::runtime_error(std::string("failed to add fd to epoll: ") + strerror(errno));
		}

		m_handlers[fd] = HandlerEntry(handler, events);
	}

	void EpollMultiplexer::modify(int32_t fd, uint32_t events) {
		if (m_epollFd == -1) {
			throw std::runtime_error("multiplexer not initialized");
		}

		if (fd < 0) {
			throw std::runtime_error("invalid fd");
		}

		HandlerMap::iterator it = m_handlers.find(fd);
		if (it == m_handlers.end()) {
			throw std::runtime_error("fd not registered");
		}

		HandlerEntry& entry = it->second;
		entry.events = events;

		struct epoll_event ev;
		std::memset(&ev, 0, sizeof(ev));
		ev.events = convertToEpollEvents(events);
		ev.data.fd = fd;

		if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
			throw std::runtime_error(std::string("failed to modify epoll event: ") + strerror(errno));
		}
	}

	void EpollMultiplexer::unsubscribe(int32_t fd) {
		if (fd < 0) {
			throw std::runtime_error("invalid fd");
		}

		HandlerMap::iterator it = m_handlers.find(fd);
		if (it == m_handlers.end()) {
			return;
		}

		struct epoll_event ev;
		std::memset(&ev, 0, sizeof(ev));

		if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev) == -1) {
			if (errno == EBADF) { // fd might have been already closed so we ignore this
				m_handlers.erase(it);
				return;
			}
			throw std::runtime_error(std::string("failed to remove fd from epoll: ") + strerror(errno));
		}

		m_handlers.erase(it);
	}

	int32_t EpollMultiplexer::dispatch(int32_t timeoutMs) {
		if (m_handlers.empty()) {
			return 0;
		}

		epoll_event events[MAX_EVENTS];
		int32_t nfds = epoll_wait(m_epollFd, events, MAX_EVENTS, timeoutMs);
		if (nfds == -1) {
			if (errno == EINTR) {
				return 0;
			}
			throw std::runtime_error(std::string("epoll_wait failed: ") + strerror(errno));
		}

		for (int32_t i = 0; i < nfds; ++i) {
			int32_t fd = events[i].data.fd;
			HandlerMap::iterator it = m_handlers.find(fd);

			if (it == m_handlers.end()) {
				continue;
			}

			AHandler* handler = it->second.handler;
			uint32_t currentEvents = events[i].events;
			try {
				if (currentEvents & EPOLLERR) {
					if (handler->onError() == HANDLE_COMPLETE) {
						unsubscribe(fd);
						continue;
					}
				}

				if (currentEvents & EPOLLIN) {
					if (handler->onReadable() == HANDLE_COMPLETE) {
						unsubscribe(fd);
						continue;
					}
				}

				if (currentEvents & EPOLLOUT) {
					if (handler->onWriteable() == HANDLE_COMPLETE) {
						unsubscribe(fd);
						continue;
					}
				}
			} catch (const std::exception& e) {
				LOG_ERROR(std::string("handler exception: ") + e.what());

				try {
					unsubscribe(fd);
				} catch (...) {}
			}
		}

		return nfds;
	}

	uint32_t EpollMultiplexer::convertToEpollEvents(uint32_t events) const {
		uint32_t epollEvents = 0;

		if (events & EVENT_READ) {
			epollEvents |= EPOLLIN;
		}
		if (events & EVENT_WRITE) {
			epollEvents |= EPOLLOUT;
		}
		if (events & EVENT_ERROR) {
			epollEvents |= EPOLLERR;
		}

		return epollEvents;
	}

} /* namespace io */

#endif // platform check