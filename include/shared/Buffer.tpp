#include <iostream>
#include <stdexcept>

namespace shared {

	/**
	 * @brief Constructs a new Buffer object.
	 */
	template <std::size_t Capacity>
	Buffer<Capacity>::Buffer()
		: m_readPos(0)
		, m_writePos(0) {
		std::memset(m_data, 0, Capacity);
	}

	/**
	 * @brief Destroys the Buffer object.
	 */
	template <std::size_t Capacity>
	Buffer<Capacity>::~Buffer() {
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::compact() {
		if (m_readPos == 0) {
			return;
		}

		std::size_t unreadSize = m_writePos - m_readPos;
		if (unreadSize > 0) {
			std::memmove(m_data, m_data + m_readPos, unreadSize);
		}
		m_writePos = unreadSize;
		m_readPos = 0;
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::append(const char* data, std::size_t size) {
		if (m_writePos + size > Capacity) {
			this->compact();
			if (m_writePos + size > Capacity) {
				throw std::runtime_error("buffer full");
			}
		}
		std::memcpy(m_data + m_writePos, data, size);
		this->advanceWriter(size);
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::consume(std::size_t size) {
		if (size > this->size()) {
			throw std::out_of_range("cannot consume more data than available");
		}
		m_readPos += size;
	}

	template <std::size_t Capacity>
	std::size_t Buffer<Capacity>::prepareWrite() {
		if (m_writePos == Capacity) {
			compact();
		}

		return Capacity - m_writePos;
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::advanceWriter(std::size_t size) {
		if (size > availableSpace()) {
			throw std::runtime_error("write advancement too large");
		}
		m_writePos += size;
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::reset() {
		m_readPos = 0;
		m_writePos = 0;
		std::memset(m_data, 0, Capacity);
	}

	template <std::size_t Capacity>
	std::size_t Buffer<Capacity>::size() const { return m_writePos - m_readPos; }

	template <std::size_t Capacity>
	std::size_t Buffer<Capacity>::capacity() const { return Capacity; }

	template <std::size_t Capacity>
	std::size_t Buffer<Capacity>::availableSpace() const {
		return Capacity - m_writePos;
	}

	template <std::size_t Capacity>
	bool Buffer<Capacity>::isFull() const {
		return m_writePos == Capacity;
	}

	template <std::size_t Capacity>
	bool Buffer<Capacity>::isEmpty() const {
		return m_readPos == m_writePos;
	}

	template <std::size_t Capacity>
	char* Buffer<Capacity>::getReadPos() { return m_data + m_readPos; }

	template <std::size_t Capacity>
	char* Buffer<Capacity>::getWritePos() { return m_data + m_writePos; }

} /* namespace shared */