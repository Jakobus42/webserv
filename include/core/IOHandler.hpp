#pragma once

#include "core/AHandler.hpp"
#include "core/RequestHandler.hpp"
#include "core/ResponseHandler.hpp"

namespace core {

	/**
	 * @class IOHandler
	 * @brief ...
	 */
	class IOHandler : public AHandler {
		public:
			IOHandler();
			~IOHandler();
			IOHandler(const IOHandler &other);
			IOHandler &operator=(const IOHandler &rhs);

			void handle(HandlerContext &ctx);
			bool hasCompleted() const;
		private:
			RequestHandler m_requestHandler;
			ResponseHandler m_responseHandler;
	};

} /* namespace core */
