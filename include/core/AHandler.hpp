#pragma once

#include <stdint.h>

#include "shared/NonCopyable.hpp"

namespace core {

	/**
	 * @class AHandler
	 * @brief ...
	 */
	class AHandler : public shared::NonCopyable {
		public:
			AHandler()
				: m_isDone(false) {}

			virtual ~AHandler() {}

			virtual void handle(int32_t fd, uint32_t events) = 0;

			bool isDone() const { return m_isDone; }

		protected:
			void markDone() { m_isDone = true; }

		private:
			bool m_isDone;
	};

} /* namespace core */
