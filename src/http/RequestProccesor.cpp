#include "http/RequestProccesor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new RequestProccesor object.
	 */
	RequestProccesor::RequestProccesor() {
		m_handlers.insert(std::make_pair(GET, new GetHandler()));
		m_handlers.insert(std::make_pair(POST, new PostHandler()));
		m_handlers.insert(std::make_pair(DELETE, new DeleteHandler()));
	}

	/**
	 * @brief Destroys the RequestProccesor object.
	 */
	RequestProccesor::~RequestProccesor() {
		for (std::map<Method, IRequestHandler*>::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
	}

	Response* RequestProccesor::process(const Request&) {
		std::cout << "processing" << std::endl;
		return new Response();
	}

} /* namespace http */
