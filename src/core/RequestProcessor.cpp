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

#include <algorithm>

namespace core {

	RequestProcessor::RequestProcessor(const config::ServerConfig& serverConfig, io::Dispatcher& dispatcher)
		: m_serverConfig(serverConfig)
		, m_cgiProcessor(dispatcher, serverConfig)
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

	bool RequestProcessor::shouldRedirect(const http::Request& request) const {
		const config::LocationConfig& location = *m_router.getResult().location;
		const std::string& uriPath = request.getUri().getPath();

		if (location.path == "/" && std::count(uriPath.begin(), uriPath.end(), '/') == 1) {
			return false;
		}
		return location.hasRedirect();
	}

	bool RequestProcessor::handleFetchRequest(const http::Request& request) {
		ARequestHandler* handler = m_handlers[request.getMethod()];

		if (handler->needsRoute()) {
			const std::string& path = request.getUri().getPath();
			m_router.route(shared::string::StringView(path.c_str(), path.size()), m_serverConfig.location);
			if (shouldRedirect(request)) {
				generateRedirectResponse();
				return false;
			}
			if (m_router.methodIsAllowed(request.getMethod()) == false) {
				throw http::HttpException(http::METHOD_NOT_ALLOWED, "HTTP method not allowed for the targeted location");
			}
			handler->setRoute(m_router.getResult());
		}
		return handler->handle(request, *m_response);
	}

	bool RequestProcessor::handleCGIRequest(const http::Request& request) {
		if (m_cgiProcessor.process(request)) {
			return true;
		}
		delete m_response;
		m_response = m_cgiProcessor.releaseResponse();
		return false;
	}

	bool RequestProcessor::processRequest(const http::Request& request) {
		if (!m_response) {
			m_response = new http::Response();
			if (request.isValid() == false) { // yeah this is kinda weird...
				generateErrorResponse(request.getStatusCode());
				return false;
			}
		}
		try {
			http::Request::Type requestType = request.getType();

			if (requestType == http::Request::FETCH) {
				return handleFetchRequest(request);
			}
			if (requestType == http::Request::CGI) {
				return handleCGIRequest(request);
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

	// todo: generate body
	void RequestProcessor::generateErrorResponse(http::StatusCode statusCode) {
		m_response->appendBody("tmp error response\n");
		m_response->setStatusCode(statusCode);
	}

	// todo: remove body
	// todo: check if appendHeader works instead of constructing
	//       temporary locationHeader vector
	void RequestProcessor::generateRedirectResponse() {
		const Route& route = m_router.getResult();


		m_response->appendHeader("Location", route.redirectUri);
		m_response->setStatusCode(route.location->returnClass);
	}

} // namespace core
