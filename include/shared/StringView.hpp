#pragma once

#include <stdint.h>

#include <iostream>

namespace shared {

	class StringView {
		public:
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

			bool operator==(const StringView& rhs) const;
			bool operator!=(const StringView& rhs) const;
			bool operator<(const StringView& other) const;

			friend std::ostream& operator<<(std::ostream& os, const StringView& sv);

		private:
			const char* m_data;
			uint32_t m_size;
	};

} // namespace shared