#pragma once

#include <cstring>
#include <stdexcept>

namespace shared {

	template <std::size_t Capacity>
	class Buffer {
		public:
			Buffer()
				: m_readPos(0)
				, m_writePos(0) {}

			~Buffer() {}

			void compact() {
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

			std::size_t prepareWrite(std::size_t minSpace = 1) {
				if (m_writePos + minSpace > Capacity) {
					compact();
					if (m_writePos + minSpace > Capacity) {
						return 0;
					}
				}
				return Capacity - m_writePos;
			}

			void append(const char* data, std::size_t size) {
				if (prepareWrite(size) < size) {
					throw std::runtime_error("buffer full");
				}
				std::memcpy(m_data + m_writePos, data, size);
				m_writePos += size;
			}

			const char* find(const char* pattern, std::size_t patternLength) const {
				if (isEmpty() || patternLength == 0 || patternLength > size()) {
					return NULL;
				}

				const char* bufferEnd = writePtr() - patternLength + 1;
				for (const char* p = readPtr(); p < bufferEnd; ++p) {
					if (std::memcmp(p, pattern, patternLength) == 0) {
						return p;
					}
				}
				return NULL;
			}

			const char* find(const char* pattern) const { return find(pattern, std::strlen(pattern)); }

			void consume(std::size_t size) {
				if (size > this->size()) {
					throw std::out_of_range("cannot consume more data than available");
				}
				m_readPos += size;
			}

			void advanceWriter(std::size_t size) {
				if (size > Capacity - m_writePos) {
					throw std::runtime_error("write advancement too large");
				}
				m_writePos += size;
			}

			void clear() {
				m_readPos = 0;
				m_writePos = 0;
			}

			std::size_t size() const { return m_writePos - m_readPos; }

			std::size_t capacity() const { return Capacity; }

			std::size_t availableSpace() const { return Capacity - m_writePos; }

			bool isFull() const { return m_writePos == Capacity; }

			bool isEmpty() const { return m_readPos == m_writePos; }

			const char* readPtr() const { return m_data + m_readPos; }

			char* readPtr() { return m_data + m_readPos; }

			const char* writePtr() const { return m_data + m_writePos; }

			char* writePtr() { return m_data + m_writePos; }

		private:
			char m_data[Capacity];
			std::size_t m_readPos;
			std::size_t m_writePos;
	};

} /* namespace shared */