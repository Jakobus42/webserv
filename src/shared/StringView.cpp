#include "shared/StringView.hpp"

#include <iostream>

namespace shared {

	/**
	 * @brief Constructs a new StringView object.
	 */
	template <typename CharT>
	StringView<CharT>::StringView()
		: m_data(NULL)
		, m_size(0) {
	}

	template <typename CharT>
	StringView<CharT>::StringView(const char* data, uint32_t size)
		: m_data(data)
		, m_size(size) {}

	/**
	 * @brief Destroys the StringView object.
	 */
	template <typename CharT>
	StringView<CharT>::~StringView() {}

	/**
	 * @brief Copy constructor.
	 * @param other The other StringView object to copy.
	 */
	template <typename CharT>
	StringView<CharT>::StringView(const StringView& other)
		: m_data(other.m_data)
		, m_size(other.m_size) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other StringView object to assign from.
	 * @return A reference to the assigned StringView object.
	 */
	template <typename CharT>
	StringView<CharT>& StringView<CharT>::operator=(const StringView<CharT>& rhs) {
		if (this != &rhs) {
			m_data = rhs.m_data;
			m_size = rhs.m_size;
		}
		return *this;
	}

	template <typename CharT>
	const CharT* StringView<CharT>::data() const { return m_data; }

	template <typename CharT>
	uint32_t StringView<CharT>::size() const { return m_size; }


} /* namespace shared */
