#pragma once

namespace core {

	/**
	 * @class IHandler
	 * @brief ...
	 */
	class IHandler {
		public:
			IHandler();
			virtual ~IHandler();

			virtual void handle(int32_t fd, uint32_t events) = 0;

		private:
			IHandler(const IHandler& other);
			IHandler& operator=(const IHandler& rhs);
	};

} /* namespace core */
