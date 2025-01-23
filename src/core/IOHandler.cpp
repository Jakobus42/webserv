#include "core/IOHandler.hpp"

#include <sys/socket.h>

#include "shared/stringUtils.hpp"

namespace core {

	/**
	 * @brief Constructs a new IOHandler object.
	 */
	IOHandler::IOHandler(http::VirtualServer& vServer)
		: m_vServer(vServer)
		, m_responses() {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
		for (std::deque<http::Response*>::iterator it = m_responses.begin(); it != m_responses.end(); ++it) {
			delete *it;
		}
	}

	void IOHandler::handle(int32_t fd, uint32_t events) {
		if (events & EPOLLERR || events & EPOLLHUP) {
			m_vServer.log("epoll error or hangup detected on client socket", shared::ERROR); // todo maybe more detailed logs but idc rn lol
			m_vServer.dropClient(fd);
			return this->markDone();
		}

		try {
			if (events & EPOLLIN) {
				this->handleRead(fd);
			}

			if (events & EPOLLOUT) {
				this->handleWrite(fd);
			}
		} catch (const std::exception& e) {
			m_vServer.log(e.what(), shared::ERROR);
			m_vServer.dropClient(fd);
			return this->markDone();
		}
	}

	void IOHandler::handleRead(int32_t fd) {
		shared::Buffer<REQUEST_BUFFER_SIZE>& buff = m_reqParser.getWriteBuffer();
		buff.prepareWrite();

		ssize_t bytesRead = recv(fd, buff.getWritePos(), buff.availableSpace(), 0);
		if (bytesRead == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return;
			}
			throw std::runtime_error("recv() failed");
		}
		if (bytesRead == 0) {
			if (m_keepAlive == false) {
				m_vServer.dropClient(fd);
				return this->markDone();
			}
		}
		buff.advanceWriter(bytesRead);

		m_reqParser.process();
		if (m_reqParser.isComplete() || m_reqParser.hasError()) {
			const http::Request& req = m_reqParser.getRequest();
			m_keepAlive = req.keepAlive();

			http::Response* res = m_reqProccesor.process(m_reqParser.getRequest());
			res->serialize();
			m_responses.push_back(res);
			m_reqParser.reset();
		}
	}

	void IOHandler::handleWrite(int32_t fd) {
		if (m_responses.empty()) { // todo this is a dirty fix for EPOLLOUT being triggered twice if connection keep alive.
			return;
		}

		shared::Buffer<RESPONSE_BUFFER_SIZE>& buff = m_responses.front()->getData();
		ssize_t bytesSent = send(fd, buff.getReadPos(), buff.size(), 0);
		if (bytesSent == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return;
			}
			throw std::runtime_error("send() failed");
		}

		buff.consume(bytesSent);
		if (buff.isEmpty()) {
			delete m_responses.front();
			m_responses.pop_front();
			if (m_keepAlive == false) {
				m_vServer.dropClient(fd);
				this->markDone();
			}
		}
	}

} /* namespace core */
