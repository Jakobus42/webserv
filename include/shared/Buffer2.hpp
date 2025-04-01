#pragma once

#include "shared/NonCopyable.hpp"

#include <cstddef>
#include <stdexcept>

namespace shared {

	template <std::size_t Capacity>
	class Buffer : shared::NonCopyable {
		public:
			Buffer()
				: m_readPos(0)
				, m_writePos(0)
				, m_size(0) {}

			~Buffer() {}

			void skip(std::size_t size) {
				if (size > m_size) {
					throw std::out_of_range("cannot skip more data than available");
				}

				m_readPos = (m_readPos + size) % Capacity;
				m_size -= size;
				if (m_size == 0) {
					clear();
				}
			}

			std::size_t contiguousReadSize() const {
				if (isEmpty()) {
					return 0;
				}

				if (m_readPos < m_writePos) {
					return m_writePos - m_readPos;
				} else {
					return Capacity - m_readPos;
				}
			}

			std::size_t contiguousWriteSize() const {
				if (isFull()) {
					return 0;
				}

				if (m_readPos <= m_writePos) {
					return Capacity - m_writePos;
				} else {
					return m_readPos - m_writePos;
				}
			}

			void clear() {
				m_readPos = 0;
				m_writePos = 0;
				m_size = 0;
			}

			std::size_t size() const { return m_size; }

			std::size_t capacity() const { return Capacity; }

			std::size_t availableSpace() const { return Capacity - m_size; }

			bool isFull() const { return m_size == Capacity; }

			bool isEmpty() const { return m_size == 0; }

			const char* readPtr() const { return m_data + m_readPos; }

			char* readPtr() { return m_data + m_readPos; }

			const char* writePtr() const { return m_data + m_writePos; }

			char* writePtr() { return m_data + m_writePos; }

			void commitWrite(std::size_t size) {
				if (m_size + size > Capacity) {
					throw std::runtime_error("commit size too large");
				}

				m_writePos = (m_writePos + size) % Capacity;
				m_size += size;
			}

		private:
			char m_data[Capacity];
			std::size_t m_readPos;
			std::size_t m_writePos;
			std::size_t m_size;
	};

} /* namespace shared */