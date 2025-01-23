#pragma once

#include "shared/Buffer.hpp"
#include "shared/NonCopyable.hpp"

#define RESPONSE_BUFFER_SIZE 16 * 1024

namespace http {

	/**
	 * @class Response
	 * @brief ...
	 */
	class Response : shared::NonCopyable {
		public:
			Response();
			~Response();

			shared::Buffer<RESPONSE_BUFFER_SIZE>& getData();

		private:
			shared::Buffer<RESPONSE_BUFFER_SIZE> m_data;
	};

} /* namespace http */
