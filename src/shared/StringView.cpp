#include "shared/StringView.hpp"

#include <cstring>

namespace shared {

	StringView::StringView()
		: m_data(NULL)
		, m_size(0) {}

	StringView::StringView(const char* data, uint32_t size)
		: m_data(data)
		, m_size(size) {}

	StringView::StringView(const char* data)
		: m_data(data)
		, m_size(std::strlen(data)) {}

	StringView::StringView(const StringView& other)
		: m_data(other.m_data)
		, m_size(other.m_size) {}

	StringView& StringView::operator=(const StringView& rhs) {
		if (this != &rhs) {
			m_data = rhs.m_data;
			m_size = rhs.m_size;
		}
		return *this;
	}

	StringView::~StringView() {}

	const char* StringView::data() const { return m_data; }

	uint32_t StringView::size() const { return m_size; }

	bool StringView::empty() const { return m_size == 0; }

	const char* StringView::begin() const { return m_data; }

	const char* StringView::end() const { return m_data + m_size; }

	void StringView::remove_prefix(uint32_t n) {
		if (n > m_size) {
			n = m_size;
		}
		m_data += n;
		m_size -= n;
	}

	void StringView::remove_suffix(uint32_t n) {
		if (n > m_size) {
			n = m_size;
		}
		m_size -= n;
	}

	StringView StringView::substr(uint32_t pos, uint32_t count) const {
		if (pos >= m_size) {
			return StringView();
		}

		uint32_t rcount = std::min(count, m_size - pos);
		return StringView(m_data + pos, rcount);
	}

	uint32_t StringView::find(const StringView& v, uint32_t pos) const {
		if (v.empty()) {
			return pos <= m_size ? pos : npos;
		}

		if (pos >= m_size || m_size - pos < v.m_size) {
			return npos;
		}

		const char* end = m_data + m_size - v.m_size + 1;
		for (const char* p = m_data + pos; p < end; ++p) {
			if (std::memcmp(p, v.m_data, v.m_size) == 0) {
				return p - m_data;
			}
		}
		return npos;
	}

	uint32_t StringView::find(char ch, uint32_t pos) const {
		if (pos >= m_size) {
			return npos;
		}

		for (uint32_t i = pos; i < m_size; ++i) {
			if (m_data[i] == ch) {
				return i;
			}
		}
		return npos;
	}

	uint32_t StringView::rfind(const StringView& v, uint32_t pos) const {
		if (v.empty()) {
			return pos <= m_size ? pos : m_size;
		}

		if (m_size < v.m_size) {
			return npos;
		}

		if (pos > m_size - v.m_size) {
			pos = m_size - v.m_size;
		}

		uint32_t i = pos + 1;
		do {
			--i;
			if (std::memcmp(m_data + i, v.m_data, v.m_size) == 0) {
				return i;
			}
		} while (i > 0);

		return npos;
	}

	uint32_t StringView::rfind(char ch, uint32_t pos) const {
		if (m_size == 0) {
			return npos;
		}

		if (pos >= m_size) {
			pos = m_size - 1;
		}

		uint32_t i = pos + 1;
		do {
			--i;
			if (m_data[i] == ch) {
				return i;
			}
		} while (i > 0);
		return npos;
	}

	uint32_t StringView::find_first_not_of(char ch, uint32_t pos) const {
		if (pos >= m_size) {
			return npos;
		}

		for (uint32_t i = pos; i < m_size; ++i) {
			if (m_data[i] != ch) {
				return i;
			}
		}
		return npos;
	}

	uint32_t StringView::find_first_not_of(const StringView& chars, uint32_t pos) const {
		if (pos >= m_size) {
			return npos;
		}

		for (uint32_t i = pos; i < m_size; ++i) {
			if (!contains_char(m_data[i], chars)) {
				return i;
			}
		}
		return npos;
	}

	uint32_t StringView::find_last_not_of(char ch, uint32_t pos) const {
		if (m_size == 0) {
			return npos;
		}

		if (pos >= m_size) {
			pos = m_size - 1;
		}

		uint32_t i = pos + 1;
		do {
			--i;
			if (m_data[i] != ch) {
				return i;
			}
		} while (i > 0);
		return npos;
	}

	uint32_t StringView::find_last_not_of(const StringView& chars, uint32_t pos) const {
		if (m_size == 0) {
			return npos;
		}

		if (pos >= m_size) {
			pos = m_size - 1;
		}

		uint32_t i = pos + 1;
		do {
			--i;
			if (!contains_char(m_data[i], chars)) {
				return i;
			}
		} while (i > 0);
		return npos;
	}

	std::string StringView::to_string() const { return std::string(m_data, m_size); }

	char StringView::operator[](uint32_t index) const { return m_data[index]; }

	bool StringView::operator==(const StringView& rhs) const {
		return m_size == rhs.m_size &&
			(m_data == rhs.m_data || std::memcmp(m_data, rhs.m_data, m_size) == 0);
	}

	bool StringView::operator!=(const StringView& rhs) const { return !(*this == rhs); }

	bool StringView::operator<(const StringView& other) const {
		if (m_data == other.m_data && m_size == other.m_size) {
			return false;
		}

		int cmp = std::memcmp(m_data, other.m_data, std::min(m_size, other.m_size));
		if (cmp == 0) {
			return m_size < other.m_size;
		}
		return cmp < 0;
	}

	std::ostream& operator<<(std::ostream& os, const StringView& sv) {
		for (uint32_t i = 0; i < sv.m_size; ++i) {
			os << sv.m_data[i];
		}
		return os;
	}

	bool StringView::contains_char(char ch, const StringView& chars) const {
		for (uint32_t i = 0; i < chars.m_size; ++i) {
			if (ch == chars.m_data[i]) {
				return true;
			}
		}
		return false;
	}

} // namespace shared