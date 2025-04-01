#include "shared/StringView.hpp"

#include <cstring>

namespace shared {

	StringView::StringView()
		: m_data(NULL)
		, m_size(0) {}

	StringView::StringView(const char* data, uint32_t size)
		: m_data(data)
		, m_size(size) {}

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

	StringView StringView::substr(uint32_t pos, uint32_t count) const {
		if (pos >= m_size) {
			return StringView();
		}

		uint32_t rcount = std::min(count, m_size - pos);
		return StringView(m_data + pos, rcount);
	}

	uint32_t StringView::find(StringView v, uint32_t pos) const {
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

	char StringView::operator[](uint32_t index) const { return m_data[index]; }

	bool StringView::operator==(const StringView& rhs) const { return m_size == rhs.m_size && m_data == rhs.m_data; }

	bool StringView::operator!=(const StringView& rhs) const { return !(*this == rhs); }

	bool StringView::operator<(const StringView& other) const {
		if (m_data == other.m_data && m_size == other.m_size) {
			return false;
		}

		int cmp = std::memcmp(m_data, other.m_data, std::min(m_size, other.m_size));
		if (cmp == 0) {
			return m_size < other.m_size;
		}
		return cmp < 0 ? false : true;
	}

	std::ostream& operator<<(std::ostream& os, const StringView& sv) {
		for (uint32_t i = 0; i < sv.m_size; ++i) {
			os << sv.m_data[i];
		}
		return os;
	}

} // namespace shared