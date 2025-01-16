#pragma once

#include "shared/NonCopyable.hpp"

namespace core {

	/**
	 * @class IHandler
	 * @brief ...
	 */
	class IHandler : shared::NonCopyable {
		public:
			IHandler() {}

			virtual ~IHandler() {}

			virtual void handle(int32_t fd, uint32_t events) = 0;
	};

} /* namespace core */
