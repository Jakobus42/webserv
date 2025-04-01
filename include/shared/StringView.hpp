#pragma once

#include <stdint.h>

#include <iostream>

namespace shared {

	class StringView {
		public:
			static const uint32_t npos = UINT32_MAX;

			StringView();
			StringView(const char* data, uint32_t size);
			~StringView();

			StringView(const StringView& other);
			StringView& operator=(const StringView& rhs);

			const char* data() const;
			uint32_t size() const;
			bool empty() const;

			const char* begin() const;
			const char* end() const;

			StringView substr(uint32_t pos = 0, uint32_t count = npos) const;
			uint32_t find(StringView v, uint32_t pos = 0) const;
			uint32_t find(char ch, uint32_t pos = 0) const;

			char operator[](uint32_t index) const;

			bool operator==(const StringView& rhs) const;
			bool operator!=(const StringView& rhs) const;
			bool operator<(const StringView& other) const;

			friend std::ostream& operator<<(std::ostream& os, const StringView& sv);

		private:
			const char* m_data;
			uint32_t m_size;
	};

} // namespace shared