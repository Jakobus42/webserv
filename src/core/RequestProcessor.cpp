#include "core/RequestProcessor.hpp"

#include "config/LocationConfig.hpp"
#include "config/ServerConfig.hpp"
#include "core/ARequestHandler.hpp"
#include "core/DeleteRequestHandler.hpp"
#include "core/GetRequestHandler.hpp"
#include "core/PostRequestHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"
#include "shared/StringView.hpp"
#include "shared/stringUtils.hpp"

namespace core {

	RequestProcessor::RequestProcessor(const config::ServerConfig& serverConfig, io::Dispatcher& dispatcher)
		: m_serverConfig(serverConfig)
		, m_cgiProcessor(dispatcher)
		, m_response(NULL)
		, m_handlers()
		, m_router() {}

	RequestProcessor::~RequestProcessor() {
		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_response;
	}

	void RequestProcessor::init() {
		m_handlers[http::GET] = new GetRequestHandler();
		m_handlers[http::POST] = new PostRequestHandler();
		m_handlers[http::DELETE] = new DeleteRequestHandler();
	}

	// todo maybe have a isComplete function. the return value could be confusing
	bool RequestProcessor::processRequest(const http::Request& request) {
		if (!m_response) {
			m_response = new http::Response();
			if (request.isValid() == false) { // yeah this is kinda weird...
				generateErrorResponse(request.getStatusCode());
				return false;
			}
		}

		try {
			// todo: if method is not implemented, throw NOT_IMPLEMENTED
			http::Request::Type requestType = request.getType();

			if (requestType == http::Request::FETCH) {
				ARequestHandler* handler = m_handlers[request.getMethod()];
				if (m_router.needsRoute()) {
					m_router.route(shared::string::StringView(request.getUri().getPath().c_str()), m_serverConfig.location);
					if (m_router.foundRedirect()) {
						generateRedirectResponse();
						return false;
					}
					if (m_router.methodIsAllowed(request.getMethod()) == false) {
						throw http::HttpException(http::METHOD_NOT_ALLOWED, "HTTP method not allowed for the targeted location");
					}
					handler->setFilePath(m_router.generateFilePath());
				}
				if (handler->handle(request, *m_response)) {
					return true;
				}
			} else if (requestType == http::Request::CGI) {
				if (m_cgiProcessor.process(request)) {
					return true;
				}
				delete m_response;
				m_response = m_cgiProcessor.releaseResponse();
			}
		} catch (const std::bad_alloc&) {
			throw;
		} catch (const http::HttpException& e) {
			LOG_ERROR("failed to process request: " + std::string(e.what()));
			generateErrorResponse(e.getStatusCode());
			return false;
		} catch (const std::exception& e) {
			LOG_ERROR("failed to process request: " + std::string(e.what()));
			generateErrorResponse(http::INTERNAL_SERVER_ERROR);
			return false;
		}
		return false;
	}

	http::Response* RequestProcessor::releaseResponse() {
		http::Response* released = m_response;
		m_response = NULL;
		return released;
	}

	void RequestProcessor::reset() {
		delete m_response;
		m_response = NULL;
		m_router.reset();

		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			it->second->reset();
		}

		m_cgiProcessor.reset();
	}

	void RequestProcessor::generateErrorResponse(http::StatusCode) {
		m_response->appendBody("tmp error response\n"); // todo:
	}

	// todo: add Location header with new uri
	void RequestProcessor::generateRedirectResponse() {
		m_response->appendBody("tmp redirect response\n"); // todo:
		m_response->appendBody(m_router.generateRedirectUri() + "\n");
		m_response->setStatusCode(m_router.getReturnClass());
	}

} // namespace core
