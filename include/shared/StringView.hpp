#pragma once

#include <stdint.h>

#include <iostream>

namespace shared {

	class StringView {
		public:
			static const uint32_t npos = UINT32_MAX;

			StringView();
			StringView(const char* data, uint32_t size);
			StringView(const char* data);
			~StringView();

			StringView(const StringView& other);
			StringView& operator=(const StringView& rhs);

			const char* data() const;
			uint32_t size() const;
			bool empty() const;

			const char* begin() const;
			const char* end() const;

			void remove_prefix(uint32_t n);
			void remove_suffix(uint32_t n);

			StringView substr(uint32_t pos = 0, uint32_t count = npos) const;
			uint32_t find(const StringView& v, uint32_t pos = 0) const;
			uint32_t find(char ch, uint32_t pos = 0) const;
			uint32_t rfind(const StringView& v, uint32_t pos = npos) const;
			uint32_t rfind(char ch, uint32_t pos = npos) const;
			uint32_t find_first_not_of(const StringView& chars, uint32_t pos = 0) const;
			uint32_t find_first_not_of(char ch, uint32_t pos = 0) const;
			uint32_t find_last_not_of(const StringView& chars, uint32_t pos = npos) const;
			uint32_t find_last_not_of(char ch, uint32_t pos = npos) const;

			std::string to_string() const;

			char operator[](uint32_t index) const;

			bool operator==(const StringView& rhs) const;
			bool operator!=(const StringView& rhs) const;
			bool operator<(const StringView& other) const;

			friend std::ostream& operator<<(std::ostream& os, const StringView& sv);

		private:
			bool contains_char(char ch, const StringView& chars) const;

		private:
			const char* m_data;
			uint32_t m_size;
	};

} // namespace shared