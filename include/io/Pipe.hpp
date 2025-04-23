#pragma once

#include <unistd.h>

#include "shared/NonCopyable.hpp"

#include <string>

namespace io {

	class Pipe : public shared::mixin::NonCopyable {
		public:
			Pipe();
			~Pipe();

			void open();

			ssize_t read(void* buffer, std::size_t size);
			ssize_t write(const void* buffer, std::size_t size);

			void dupReadFd(int newFd);
			void dupWriteFd(int newFd);

			void setNonBlocking(bool nonBlocking);
			void setReadNonBlocking(bool nonBlocking);
			void setWriteNonBlocking(bool nonBlocking);

			void closeReadEnd();
			void closeWriteEnd();
			void close();

			int getReadFd() const;
			int getWriteFd() const;

		private:
			int m_readFd;
			int m_writeFd;

			bool setNonBlocking(int fd, bool nonBlocking);
	};

} /* namespace io */