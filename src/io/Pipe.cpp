#include "io/Pipe.hpp"

#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <stdexcept>

namespace io {

	Pipe::Pipe()
		: m_readFd(-1)
		, m_writeFd(-1) {}

	Pipe::~Pipe() { close(); }

	void Pipe::open() {
		int fds[2];

		if (::pipe(fds) == -1) {
			throw std::runtime_error("failed to create pipe: " + std::string(std::strerror(errno)));
		}

		m_readFd = fds[0];
		m_writeFd = fds[1];
	}

	ssize_t Pipe::read(void* buffer, std::size_t size) {
		if (m_readFd == -1) {
			throw std::runtime_error("cannot read from closed pipe");
		}
		return ::read(m_readFd, buffer, size);
	}

	ssize_t Pipe::write(const void* buffer, std::size_t size) {
		if (m_writeFd == -1) {
			throw std::runtime_error("cannot write to closed pipe");
		}
		return ::write(m_writeFd, buffer, size);
	}

	void Pipe::dupReadFd(int newFd) {
		if (m_readFd == -1) {
			throw std::runtime_error("cannot dup closed read end");
		}

		if (::dup2(m_readFd, newFd) == -1) {
			throw std::runtime_error("failed to dup read fd: " + std::string(std::strerror(errno)));
		}
	}

	void Pipe::dupWriteFd(int newFd) {
		if (m_writeFd == -1) {
			throw std::runtime_error("cannot dup closed write end");
		}

		if (::dup2(m_writeFd, newFd) == -1) {
			throw std::runtime_error("failed to dup write fd: " + std::string(std::strerror(errno)));
		}
	}

	void Pipe::closeReadEnd() {
		if (m_readFd != -1) {
			::close(m_readFd);
			m_readFd = -1;
		}
	}

	void Pipe::closeWriteEnd() {
		if (m_writeFd != -1) {
			::close(m_writeFd);
			m_writeFd = -1;
		}
	}

	void Pipe::close() {
		closeReadEnd();
		closeWriteEnd();
	}

	int Pipe::getReadFd() const { return m_readFd; }

	int Pipe::getWriteFd() const { return m_writeFd; }

} /* namespace io */