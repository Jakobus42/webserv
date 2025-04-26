#pragma once

#include "io/Dispatcher.hpp"
#include "shared/NonCopyable.hpp"

namespace http {
	class Request;
	class Response;
} // namespace http

namespace core {

	class CGIProcessor : shared::mixin::NonCopyable {
		public:
			explicit CGIProcessor(io::Dispatcher& dispatcher);
			~CGIProcessor();

			bool process(http::Request& request);
			http::Response* releaseResponse();

			void reset();

		private:
			io::Dispatcher& m_dispatcher;
	};

} /* namespace core */
