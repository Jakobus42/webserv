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
		, m_reqProcessor(vServer.getRouter())
		, m_requests()
		, m_responses() {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
		while (m_responses.size()) {
			delete m_responses.front();
			m_responses.pop();
		}
		while (m_requests.size()) {
			delete m_requests.front();
			m_requests.pop();
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
				if (!m_reqProcessor.isDone() && !m_requests.empty()) {
					m_reqProcessor.process(*(m_requests.front()));
					if (m_reqProcessor.isDone()) {
						http::Response* releasedResponse = m_reqProcessor.releaseResponse();
						releasedResponse->serialize();
						m_responses.push(releasedResponse);
						delete m_requests.front();
						m_requests.pop();
					}
				} else {
					this->handleWrite(fd);
				}
			}
		} catch (const std::exception& e) {
			m_vServer.log(e.what(), shared::ERROR);
			m_vServer.dropClient(fd);
			// TODO: send 500 error response BEFORE DROPPING CLIENT
			// or does arriving in this catch() statement mean something
			// went catastrophically wrong and we might be unable
			// to even render an error response?
			return this->markDone();
		}
	}

	void IOHandler::handleRead(int32_t fd) {
		shared::Buffer<REQUEST_BUFFER_SIZE>& buff = m_reqParser.getWriteBuffer();
		buff.prepareWrite();

		ssize_t bytesRead = recv(fd, buff.getWritePos(), buff.availableSpace(), 0);
		// m_vServer.log("received " + shared::string::to_string(bytesRead) + " bytes.", shared::INFO, fd);

		if (bytesRead == -1) {
			throw std::runtime_error("recv() failed");
		}
		if (bytesRead == 0) { // fuck keep alive
			m_vServer.dropClient(fd);
			return this->markDone();
		}
		// m_vServer.log("received request from client:\n" + std::string(buff.getWritePos(), bytesRead), shared::INFO, fd);
		buff.advanceWriter(bytesRead);

		m_reqParser.process();
		if (m_reqParser.isComplete() || m_reqParser.hasError()) {
			m_requests.push(m_reqParser.releaseRequest());
			m_reqParser.reset();
		}

		m_vServer.updateClientActivity(fd);
	}

	void IOHandler::handleWrite(int32_t fd) {
		if (m_responses.empty()) {
			return;
		}
		http::Response& res = *m_responses.front();
		std::size_t bytesToRead = res.getData().size() - res.getReadPos();

		ssize_t bytesSent = send(fd, res.getData().data() + res.getReadPos(), std::min(bytesToRead, this->getChunkSize()), 0);
		if (bytesSent == -1) {
			throw std::runtime_error("send() failed");
		}

		// m_vServer.log("Bytes sent: " + shared::string::fromNum(bytesSent), shared::INFO);

		res.advanceReadPos(bytesSent);
		if (res.eof()) {
			// m_vServer.log("Sent response:\n------\n" + res.getData() + "------\n", shared::INFO);
			delete m_responses.front();
			m_responses.pop();

			m_vServer.dropClient(fd);
			m_reqProcessor.reset();
			return this->markDone();
		}

		m_vServer.updateClientActivity(fd);
	}

} /* namespace core */
