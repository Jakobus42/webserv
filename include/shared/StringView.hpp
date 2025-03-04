#pragma once

#include <stdint.h>

namespace shared {

/**
 * @class StringView
 * @brief ...
 */
template <typename CharT>
class StringView {
    public:
        StringView();
        StringView(const char* data, uint32_t size);
        ~StringView();
        StringView(const StringView &other);
        StringView& operator=(const StringView &rhs);

        const CharT* data() const;
        uint32_t size() const;

    private:
        const CharT* m_data;
        uint32_t m_size;
};

} /* namespace shared */
