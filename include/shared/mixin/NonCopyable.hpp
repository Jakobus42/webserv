#pragma once

namespace shared {
	namespace mixin {

		class NonCopyable {
			protected:
				NonCopyable() {}

				virtual ~NonCopyable() {}

			private:
				NonCopyable& operator=(const NonCopyable& rhs);
				NonCopyable(const NonCopyable& other);
		};

	} // namespace mixin
} // namespace shared
