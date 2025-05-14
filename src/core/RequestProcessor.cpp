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

	RequestProcessor::RequestProcessor(const config::Config::ServerConfigs& serverConfigs, io::Dispatcher& dispatcher)
		: m_serverConfigs(serverConfigs)
		, m_serverConfig(serverConfigs.at(0))
		, m_cgiProcessor(dispatcher, m_serverConfig) // todo: pass global max_xxx stuff
		, m_response(NULL)
		, m_handlers()
		, m_router()
		, m_state(PREPROCESS) {}

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

	void RequestProcessor::resolveHost(const http::Request& request) {
		const std::string& host = request.getUri().getAuthority();

		for (std::size_t i = 0; i < m_serverConfigs.size(); ++i) {
			config::ServerConfig serverConfig = m_serverConfigs[i];
			for (std::size_t j = 0; j < serverConfig.serverNames.size(); ++j) {
				if (!shared::string::CaseInsensitiveComparator()(serverConfig.serverNames[j], host)) {
					m_serverConfig = serverConfig;
					return;
				}
			}
		}
		m_serverConfig = m_serverConfigs.at(0);
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

	void RequestProcessor::preprocess(const http::Request& request) {
		m_response = new http::Response();

		if (request.hasHeader("Connection") && request.getHeader("Connection").front() == "close") {
			m_response->setHeader("Connection", "close");
		} else {
			m_response->setHeader("Connection", "keep-alive");
		}

		resolveHost(request);
	}

	bool RequestProcessor::process(const http::Request& request) {
		if (request.isValid() == false) {
			generateErrorResponse(request.getStatusCode());
			return false;
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

	bool RequestProcessor::processRequest(const http::Request& request) {
		switch (m_state) {
			case PREPROCESS: {
				preprocess(request);
				m_state = PROCESS;
				break;
			}
			case PROCESS: {
				if (!process(request)) {
					m_state = DONE;
				}
				break;
			}
			case DONE: break;
		}

		return !(m_state == DONE);
	}

	http::Response* RequestProcessor::releaseResponse() {
		http::Response* released = m_response;
		m_response = NULL;
		return released;
	}

	void RequestProcessor::reset() {
		m_state = PREPROCESS;

		delete m_response;
		m_response = NULL;
		m_router.reset();

		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			it->second->reset();
		}

		m_cgiProcessor.reset();
		m_serverConfig = m_serverConfigs.at(0);
	}

	// todo: generate body
	void RequestProcessor::generateErrorResponse(http::StatusCode statusCode) {
		m_response->setHeader("Connection", "close");
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
