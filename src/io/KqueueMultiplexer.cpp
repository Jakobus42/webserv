#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#	include "io/KqueueMultiplexer.hpp"

#	include <sys/event.h>
#	include <cstring>
#	include <cerrno>
#	include <unistd.h>

namespace io {

	KqueueMultiplexer::KqueueMultiplexer()
		: AMultiplexer()
		, m_kqueueFd(-1) {
		m_kqueueFd = kqueue();
		if (m_kqueueFd == -1) {
			throw std::runtime_error(std::string("failed to create kqueue: ") + std::strerror(errno));
		}
	}

	KqueueMultiplexer::~KqueueMultiplexer() {
		if (m_kqueueFd != -1) {
			close(m_kqueueFd);
		}
	}

	void KqueueMultiplexer::add(int32_t fd, uint32_t events) {
		uint32_t nChanges = 0;
		struct kevent changes[2];
		EV_SET(&changes[0], 0, 0, 0, 0, 0, NULL);
		EV_SET(&changes[1], 0, 0, 0, 0, 0, NULL);

		if (events & EVENT_READ) {
			EV_SET(&changes[nChanges++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		}
		if (events & EVENT_WRITE) {
			EV_SET(&changes[nChanges++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		}

		if (nChanges > 0) {
			if (kevent(m_kqueueFd, changes, nChanges, NULL, 0, NULL) == -1) {
				throw std::runtime_error(std::string("failed to add fd to kqueue: ") + std::strerror(errno));
			}
		}

		m_registeredEvents[fd] = events;
	}

	void KqueueMultiplexer::modify(int32_t fd, uint32_t events) {
		EventMap::iterator it = m_registeredEvents.find(fd);
		if (it == m_registeredEvents.end()) {
			throw std::runtime_error("fd not registered");
		}

		uint32_t currentEvents = it->second;
		if (currentEvents == events) {
			return;
		}

		uint32_t nChanges = 0;
		struct kevent changes[2];
		EV_SET(&changes[0], 0, 0, 0, 0, 0, NULL);
		EV_SET(&changes[1], 0, 0, 0, 0, 0, NULL);

		if ((events & EVENT_READ) && !(currentEvents & EVENT_READ)) {
			EV_SET(&changes[nChanges++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		} else if (!(events & EVENT_READ) && (currentEvents & EVENT_READ)) {
			EV_SET(&changes[nChanges++], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		}

		if ((events & EVENT_WRITE) && !(currentEvents & EVENT_WRITE)) {
			EV_SET(&changes[nChanges++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
		} else if (!(events & EVENT_WRITE) && (currentEvents & EVENT_WRITE)) {
			EV_SET(&changes[nChanges++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		}

		if (nChanges > 0) {
			if (kevent(m_kqueueFd, changes, nChanges, NULL, 0, NULL) == -1) {
				throw std::runtime_error(std::string("failed to modify kqueue event: ") + std::strerror(errno));
			}
		}

		it->second = events;
	}

	void KqueueMultiplexer::remove(int32_t fd) {
		EventMap::iterator it = m_registeredEvents.find(fd);
		if (it == m_registeredEvents.end()) {
			return;
		}

		uint32_t currentEvents = it->second;
		uint32_t nChanges = 0;
		struct kevent changes[2];
		EV_SET(&changes[0], 0, 0, 0, 0, 0, NULL);
		EV_SET(&changes[1], 0, 0, 0, 0, 0, NULL);

		if (currentEvents & EVENT_READ) {
			EV_SET(&changes[nChanges++], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		}
		if (currentEvents & EVENT_WRITE) {
			EV_SET(&changes[nChanges++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		}

		if (nChanges > 0) {
			if (kevent(m_kqueueFd, changes, nChanges, NULL, 0, NULL) == -1) {
				if (errno != EBADF) { // Ignore EBADF as fd might have been already closed
					throw std::runtime_error(std::string("failed to remove fd from kqueue: ") + std::strerror(errno));
				}
			}
		}

		m_registeredEvents.erase(it);
	}

	int32_t KqueueMultiplexer::poll(int32_t timeoutMs) {
		m_readyEvents.clear();

		struct timespec timeout;
		struct timespec* timeoutPtr = NULL;
		if (timeoutMs != TIMEOUT_INFINITE) {
			timeout.tv_sec = timeoutMs / 1000;
			timeout.tv_nsec = (timeoutMs % 1000) * 1000000;
			timeoutPtr = &timeout;
		}

		struct kevent events[MAX_EVENTS];
		int32_t nEvents = kevent(m_kqueueFd, NULL, 0, events, MAX_EVENTS, timeoutPtr);
		if (nEvents == -1) {
			if (errno == EINTR) {
				return 0;
			}
			throw std::runtime_error(std::string("kevent failed: ") + std::strerror(errno));
		}

		for (int32_t i = 0; i < nEvents; ++i) {
			int32_t fd = events[i].ident;
			int16_t filter = events[i].filter;
			uint16_t flags = events[i].flags;

			uint32_t eventTypes = convertFromKqueueEvents(filter, flags);
			m_readyEvents.push_back(Event(fd, eventTypes));
		}

		return nEvents;
	}

	uint32_t KqueueMultiplexer::convertFromKqueueEvents(int16_t filter, uint16_t flags) const {
		uint32_t events = 0;

		if (filter == EVFILT_READ) {
			events |= EVENT_READ;
		}
		if (filter == EVFILT_WRITE) {
			events |= EVENT_WRITE;
		}
		if (flags & EV_ERROR) {
			events |= EVENT_ERROR;
		}

		return events;
	}

} /* namespace io */

#endif // platform check