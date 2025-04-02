#pragma once

#include <stdint.h>

#include <climits>
#include <iostream>

namespace shared {

	class StringView {
		public:
			static const std::size_t npos = SIZE_MAX;

			StringView();
			StringView(const char* data, std::size_t size);
			StringView(const char* data);
			~StringView();

			StringView(const StringView& other);
			StringView& operator=(const StringView& rhs);

			const char* data() const;
			std::size_t size() const;
			bool empty() const;

			const char* begin() const;
			const char* end() const;

			void remove_prefix(std::size_t n);
			void remove_suffix(std::size_t n);

			StringView substr(std::size_t pos = 0, std::size_t count = npos) const;
			std::size_t find(const StringView& v, std::size_t pos = 0) const;
			std::size_t find(char ch, std::size_t pos = 0) const;
			std::size_t rfind(const StringView& v, std::size_t pos = npos) const;
			std::size_t rfind(char ch, std::size_t pos = npos) const;
			std::size_t find_first_not_of(const StringView& chars, std::size_t pos = 0) const;
			std::size_t find_first_not_of(char ch, std::size_t pos = 0) const;
			std::size_t find_last_not_of(const StringView& chars, std::size_t pos = npos) const;
			std::size_t find_last_not_of(char ch, std::size_t pos = npos) const;

			std::string to_string() const;

			char operator[](std::size_t index) const;

			bool operator==(const StringView& rhs) const;
			bool operator!=(const StringView& rhs) const;
			bool operator<(const StringView& other) const;

			friend std::ostream& operator<<(std::ostream& os, const StringView& sv);

		private:
			bool contains_char(char ch, const StringView& chars) const;

		private:
			const char* m_data;
			std::size_t m_size;
	};

} // namespace shared