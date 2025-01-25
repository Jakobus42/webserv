#include "http/RequestProccesor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new RequestProccesor object.
	 */
	// todo figure out how to do this shit with locations
	RequestProccesor::RequestProccesor(std::vector<config::t_location> locations)
		: m_res(NULL)
		, m_locations(locations) {
		m_handlers.insert(std::make_pair(GET, new GetHandler(locations.at(0))));
		m_handlers.insert(std::make_pair(POST, new PostHandler(locations.at(0))));
		m_handlers.insert(std::make_pair(DELETE, new DeleteHandler(locations.at(0)))); // todo router or idk:c
	}

	/**
	 * @brief Destroys the RequestProccesor object.
	 */
	RequestProccesor::~RequestProccesor() {
		for (std::map<Method, ARequestHandler*>::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_res;
	}

	// todo check if req was valid - if not send error response
	// todo check for allowed methods
	Response* RequestProccesor::process(const Request& req) {
		m_res = new Response();
		m_handlers[req.getMethod()]->handle(req, *m_res);
		m_res->setBody("Hello, World!");
		return this->releaseResponse();
	}

	Response* RequestProccesor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
	}

} /* namespace http */
