#pragma once

#include "shared/NonCopyable.hpp"

#include <cstring>
#include <iostream>

namespace shared {

	/**
	 * @class Buffer
	 * @brief ...
	 */
	template <std::size_t Capacity>
	class Buffer : shared::NonCopyable {
		public:
			Buffer();
			~Buffer();

			void compact();
			void append(const char* data, std::size_t size);
			std::size_t prepareWrite();
			void advanceWriter(std::size_t size);
			void consume(std::size_t size);
			void reset();

			std::size_t size() const;
			std::size_t capacity() const;
			std::size_t availableSpace() const;

			bool isFull() const;
			bool isEmpty() const;

			char* getReadPos();
			char* getWritePos();
			const char* getData() const;

		private:
			char m_data[Capacity];
			std::size_t m_readPos;
			std::size_t m_writePos;
	};

} /* namespace shared */

#include "shared/Buffer.tpp"
