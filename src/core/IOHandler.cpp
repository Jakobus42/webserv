#include "core/IOHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new IOHandler object.
	 */
	IOHandler::IOHandler(http::VirtualServer& vServer, http::Connection& conn)
		: m_vServer(vServer)
		, m_connection(conn) {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
	}

	void IOHandler::handle(int32_t, uint32_t) {
	}

} /* namespace core */
