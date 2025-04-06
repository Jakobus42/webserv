#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) // I had to duplicate this 3 times... (I dont know it better lol maybe make this cleaner some other time)

#	include "io/KqueueMultiplexer.hpp"

#	include <errno.h>
#	include <string.h>
#	include <sys/event.h>
#	include <sys/time.h>
#	include <sys/types.h>
#	include <unistd.h>

#	include "io/AHandler.hpp"
#	include "shared/Logger.hpp"

#	include <stdexcept>

namespace io {

	KqueueMultiplexer::KqueueMultiplexer()
		: AMultiplexer()
		, m_kqueueFd(-1) {
		m_kqueueFd = kqueue();
		if (m_kqueueFd == -1) {
			throw std::runtime_error(std::string("failed to create kqueue: ") + strerror(errno));
		}
	}

	KqueueMultiplexer::~KqueueMultiplexer() {
		if (m_kqueueFd != -1) {
			close(m_kqueueFd);
		}
	}

	void KqueueMultiplexer::subscribe(int32_t fd, uint32_t events, AHandler* handler) {
		if (!handler) {
			throw std::runtime_error("null handler provided");
		}

		if (fd < 0) {
			throw std::runtime_error("invalid fd");
		}

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

		HandlerMap::iterator it = m_handlers.find(fd);
		if (it != m_handlers.end()) {
			unsubscribe(fd);
		}

		if (nChanges > 0) {
			if (kevent(m_kqueueFd, changes, nChanges, NULL, 0, NULL) == -1) {
				throw std::runtime_error(std::string("failed to add fd to kqueue: ") + strerror(errno));
			}
		}

		m_handlers[fd] = HandlerEntry(handler, events);
	}

	void KqueueMultiplexer::modify(int32_t fd, uint32_t events) {
		if (fd < 0) {
			throw std::runtime_error("invalid fd");
		}

		HandlerMap::iterator it = m_handlers.find(fd);
		if (it == m_handlers.end()) {
			throw std::runtime_error("fd not registered");
		}

		uint32_t currentEvents = it->second.events;

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
				throw std::runtime_error(std::string("failed to modify kqueue event: ") + strerror(errno));
			}
		}

		it->second.events = events;
	}

	void KqueueMultiplexer::unsubscribe(int32_t fd) {
		if (fd < 0) {
			throw std::runtime_error("invalid fd");
		}

		HandlerMap::iterator it = m_handlers.find(fd);
		if (it == m_handlers.end()) {
			return;
		}

		uint32_t events = it->second.events;
		uint32_t nChanges = 0;
		struct kevent changes[2];
		EV_SET(&changes[0], 0, 0, 0, 0, 0, NULL);
		EV_SET(&changes[1], 0, 0, 0, 0, 0, NULL);

		if (events & EVENT_READ) {
			EV_SET(&changes[nChanges++], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
		}
		if (events & EVENT_WRITE) {
			EV_SET(&changes[nChanges++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		}

		if (nChanges > 0) {
			if (kevent(m_kqueueFd, changes, nChanges, NULL, 0, NULL) == -1) {
				if (errno == EBADF) { // fd might have been already closed so we ignore this
					m_handlers.erase(it);
					return;
				}
				throw std::runtime_error(std::string("failed to remove fd from kqueue: ") + strerror(errno));
			}
		}

		m_handlers.erase(it);
	}

	int32_t KqueueMultiplexer::dispatch(int32_t timeoutMs) {
		if (m_handlers.empty()) {
			return 0;
		}

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
			throw std::runtime_error(std::string("kevent failed: ") + strerror(errno));
		}

		for (int32_t i = 0; i < nEvents; ++i) {
			int32_t fd = events[i].ident;
			HandlerMap::iterator it = m_handlers.find(fd);

			if (it == m_handlers.end()) {
				continue;
			}

			AHandler* handler = it->second.handler;
			int16_t filter = events[i].filter;
			uint16_t flags = events[i].flags;
			try {
				if (flags & EV_ERROR) {
					if (handler->onError() == HANDLE_COMPLETE) {
						unsubscribe(fd);
						continue;
					}
				}

				if (filter == EVFILT_READ) {
					if (handler->onReadable() == HANDLE_COMPLETE) {
						unsubscribe(fd);
						continue;
					}
				}

				if (filter == EVFILT_WRITE) {
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

		return nEvents;
	}

} /* namespace io */

#endif // platform check