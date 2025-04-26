#include "shared/platform.hpp"

#if defined(PLATFORM_HAS_EPOLL)

#	include "io/EpollMultiplexer.hpp"

#	include <sys/epoll.h>
#	include <unistd.h>

#	include <cstring>
#	include <stdexcept>
#	include <cerrno>

namespace io {

	EpollMultiplexer::EpollMultiplexer()
		: AMultiplexer()
		, m_epollFd(-1) {
		// Since Linux 2.6.8, the size argument is ignored
		m_epollFd = epoll_create(1);
		if (m_epollFd == -1) {
			throw std::runtime_error(std::string("failed to create epoll instance: ") + std::strerror(errno));
		}
	}

	EpollMultiplexer::~EpollMultiplexer() {
		if (m_epollFd != -1) {
			close(m_epollFd);
		}
	}

	void EpollMultiplexer::add(int32_t fd, uint32_t events) {
		struct epoll_event ev;
		std::memset(&ev, 0, sizeof(ev));
		ev.events = convertToEpollEvents(events);
		ev.data.fd = fd;

		if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev) == -1) {
			throw std::runtime_error(std::string("failed to add fd to epoll: ") + std::strerror(errno));
		}

		m_registeredEvents[fd] = events;
	}

	void EpollMultiplexer::modify(int32_t fd, uint32_t events) {
		EventMap::iterator it = m_registeredEvents.find(fd);
		if (it == m_registeredEvents.end()) {
			throw std::runtime_error("failed to modify epoll event: fd not registered");
		}
		if (it->second == events) {
			return;
		}

		struct epoll_event ev;
		std::memset(&ev, 0, sizeof(ev));
		ev.events = convertToEpollEvents(events);
		ev.data.fd = fd;

		if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &ev) == -1) {
			throw std::runtime_error(std::string("failed to modify epoll event: ") + std::strerror(errno));
		}

		it->second = events;
	}

	void EpollMultiplexer::remove(int32_t fd) {
		EventMap::iterator it = m_registeredEvents.find(fd);
		if (it == m_registeredEvents.end()) {
			return;
		}

		struct epoll_event ev;
		std::memset(&ev, 0, sizeof(ev));

		if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev) == -1) {
			throw std::runtime_error(std::string("failed to remove fd from epoll: ") + std::strerror(errno));
		}

		m_registeredEvents.erase(it);
	}

	int32_t EpollMultiplexer::poll(int32_t timeoutMs) {
		m_readyEvents.clear();

		epoll_event events[MAX_EVENTS];
		int32_t nfds = epoll_wait(m_epollFd, events, MAX_EVENTS, timeoutMs);
		if (nfds == -1) {
			if (errno == EINTR) {
				return 0;
			}
			throw std::runtime_error(std::string("epoll_wait failed: ") + std::strerror(errno));
		}

		for (int32_t i = 0; i < nfds; ++i) {
			int32_t fd = events[i].data.fd;
			uint32_t currentEvents = convertFromEpollEvents(events[i].events);
			m_readyEvents.push_back(Event(fd, currentEvents));
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

	uint32_t EpollMultiplexer::convertFromEpollEvents(uint32_t epollEvents) const {
		uint32_t events = 0;

		if (epollEvents & EPOLLIN) {
			events |= EVENT_READ;
		}
		if (epollEvents & EPOLLOUT) {
			events |= EVENT_WRITE;
		}
		if (epollEvents & EPOLLERR) {
			events |= EVENT_ERROR;
		}

		return events;
	}

} /* namespace io */

#endif // platform check