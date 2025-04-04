#pragma once

namespace shared {

	/**
	 * @class NonCopyable
	 * @brief ...
	 */
	class NonCopyable {
		protected:
			NonCopyable() {}

			virtual ~NonCopyable() {}

		private:
			NonCopyable& operator=(const NonCopyable& rhs);
			NonCopyable(const NonCopyable& other);
	};

} /* namespace shared */
