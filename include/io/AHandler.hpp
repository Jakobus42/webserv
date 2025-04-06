#pragma once

#include "shared/NonCopyable.hpp"

namespace io {

	enum HandleResult {
		HANDLE_CONTINUE,
		HANDLE_COMPLETE
	};

	class AHandler : shared::mixin::NonCopyable {
		public:
			virtual ~AHandler();

			virtual HandleResult onReadable() { return HANDLE_CONTINUE; };

			virtual HandleResult onWriteable() { return HANDLE_CONTINUE; };

			virtual HandleResult onError() { return HANDLE_CONTINUE; };
	};

} /* namespace io */
