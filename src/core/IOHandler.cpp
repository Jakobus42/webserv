#include "core/IOHandler.hpp"

#include <sys/socket.h>

#include "shared/stringUtils.hpp"

namespace core {

	/**
	 * @brief Constructs a new IOHandler object.
	 */
	IOHandler::IOHandler(http::VirtualServer& vServer)
		: m_vServer(vServer)
		, m_reqParser()
		, m_reqProccesor(vServer.getConfig().locations)
		, m_responses()
		, m_keepAlive(false) {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
		while (m_responses.size()) {
			delete m_responses.front();
			m_responses.pop();
		}
	}

	void IOHandler::handle(int32_t fd, uint32_t events) {
		if (events & EPOLLERR || events & EPOLLHUP) {
			m_vServer.log("epoll error or hangup detected on client socket", shared::ERROR, fd);
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
			throw std::runtime_error("recv() failed");
		}
		if (bytesRead == 0) {
			if (m_keepAlive == false) {
				m_vServer.dropClient(fd);
				return this->markDone();
			} else {
				return;
			}
		}
		m_vServer.log("received request from client:\n" + std::string(buff.getWritePos(), bytesRead), shared::INFO, fd);
		buff.advanceWriter(bytesRead);

		m_reqParser.process();
		if (m_reqParser.isComplete() || m_reqParser.hasError()) {
			const http::Request& req = m_reqParser.getRequest();
			m_keepAlive = req.keepAlive(); // todo maybe dont do this at all

			http::Response* res = m_reqProccesor.process(m_reqParser.getRequest());
			res->serialize();
			m_responses.push(res);
			m_reqParser.reset();
		}

		m_vServer.updateClientActivity(fd);
	}

	void IOHandler::handleWrite(int32_t fd) {
		if (m_responses.empty()) {
			return;
		}

		shared::Buffer<RESPONSE_BUFFER_SIZE>& buff = m_responses.front()->getData();
		ssize_t bytesSent = send(fd, buff.getReadPos(), buff.size(), 0);
		if (bytesSent == -1) {
			throw std::runtime_error("send() failed");
		}

		m_vServer.log("sent response to client\n" + std::string(buff.getReadPos(), buff.getWritePos()), shared::INFO, fd);

		buff.consume(bytesSent);
		if (buff.isEmpty()) {
			delete m_responses.front();
			m_responses.pop();
			if (m_keepAlive == false) {
				m_vServer.dropClient(fd);
				return this->markDone();
			}
		}

		m_vServer.updateClientActivity(fd);
	}

} /* namespace core */
