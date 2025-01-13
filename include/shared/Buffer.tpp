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

	/**
	 * @brief Copy constructor.
	 * @param other The other Buffer object to copy.
	 */
	template <std::size_t Capacity>
	Buffer<Capacity>::Buffer(const Buffer& other)
		: m_readPos(other.m_readPos)
		, m_writePos(other.m_writePos) {
		std::memcpy(m_data, other.m_data, Capacity);
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Buffer object to assign from.
	 * @return A reference to the assigned Buffer object.
	 */
	template <std::size_t Capacity>
	Buffer<Capacity>& Buffer<Capacity>::operator=(const Buffer& rhs) {
		if (this != &rhs) {
			m_readPos = rhs.m_readPos;
			m_writePos = rhs.m_writePos;
			std::memcpy(m_data, rhs.m_data, Capacity);
		}
		return *this;
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::compact() {
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
				throw std::runtime_error("buffer overflow: not enough space to append");
			}
		}
		std::memcpy(m_data + m_writePos, data, size);
		m_writePos += size;
	}

	template <std::size_t Capacity>
	void Buffer<Capacity>::consume(std::size_t size) {
		if (size > this->size()) {
			throw std::out_of_range("cannot consume more data than available");
		}
		m_readPos += size;
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
		return Capacity - (m_writePos - m_readPos);
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