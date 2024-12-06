#pragma once

#include "core/IHandler.hpp"
#include "core/RequestHandler.hpp"
#include "core/ResponseHandler.hpp"

namespace core {

	/**
	 * @class IOHandler
	 * @brief ...
	 */
	class IOHandler : public IHandler {
		public:
			IOHandler();
			~IOHandler();
			IOHandler(const IOHandler &other);
			IOHandler &operator=(const IOHandler &rhs);

			void handle(HandleContext &ctx);
		private:
			RequestHandler m_requestHandler;
			ResponseHandler m_responseHandler;
	};

} /* namespace core */
