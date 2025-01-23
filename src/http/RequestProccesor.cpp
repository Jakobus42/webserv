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
		delete m_res;
	}

	Response* RequestProccesor::process(const Request& req) {
		m_res = new Response();
		m_handlers[req.getMethod()]->handle(req, *m_res);
		m_res->setHeader("Content-Length", "0");
		m_res->setCode(NO_CONTENT);
		return this->releaseResponse();
	}

	Response* RequestProccesor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
	}

} /* namespace http */
