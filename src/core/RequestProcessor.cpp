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
		, m_route(NULL)
		, m_handlers() {
		m_handlers[http::GET] = new GetRequestHandler();
		m_handlers[http::POST] = new PostRequestHandler();
		m_handlers[http::DELETE] = new DeleteRequestHandler();
	}

	RequestProcessor::~RequestProcessor() {
		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_response;
		delete m_route;
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
			http::Request::Type requestType = request.getType();

			if (requestType == http::Request::FETCH) {
				if (m_route == NULL) {
					m_route = RequestProcessor::route(shared::string::StringView(request.getUri().getPath().c_str()), m_serverConfig.location);
				}
				if (m_route->location.hasRedirect()) {
					generateRedirectResponse();
					return false;
				}
				ARequestHandler* handler = m_handlers[request.getMethod()];
				if (handler->handle(request, *m_response)) {
					return true;
				}
				m_response->appendBody("tmp response\n"); // tmp
			} else if (requestType == http::Request::CGI) {
				if (m_cgiProcessor.process(request)) {
					return true;
				}
				delete m_response;
				m_response = m_cgiProcessor.releaseResponse();
			}
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
		delete m_route;
		m_route = NULL;
		return released;
	}

	void RequestProcessor::reset() {
		delete m_response;
		m_response = NULL;
		delete m_route;
		m_route = NULL;

		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			it->second->reset();
		}

		m_cgiProcessor.reset();
	}

	void RequestProcessor::generateErrorResponse(http::StatusCode) {
		m_response->appendBody("tmp error response\n"); // todo:
	}

	void RequestProcessor::generateRedirectResponse() {
		m_response->appendBody("tmp redirect response\n"); // todo:
		m_response->appendBody(m_route->location.redirectUri + m_route->filePath + "\n");
		m_response->setStatusCode(m_route->location.returnClass);
	}

	RouteResult* RequestProcessor::route(const shared::string::StringView& uriPath, const config::LocationConfig& currentLocation, std::size_t depth) {
		std::cout << "routing to '" << uriPath << "' in location '" << currentLocation.path << "'" << std::endl;
		if (currentLocation.hasRedirect()) {
			return new RouteResult(currentLocation, uriPath.toString());
		}
		if (depth > MAX_ROUTE_DEPTH) {
			throw http::HttpException(http::LOOP_DETECTED, "Depth exceeded MAX_ROUTE_DEPTH");
		}
		for (std::vector<config::LocationConfig>::const_iterator location = currentLocation.locations.begin(); location != currentLocation.locations.end(); ++location) {
			shared::string::StringView pathView(location->path.c_str());
			if (!pathView.empty() && uriPath.find(pathView, 0) == 0) {
				return route(uriPath.substr(pathView.size()), *location, depth + 1);
			}
		}
		return new RouteResult(currentLocation, uriPath.toString());
	}

	// returns pair of location & file name (including subdirectories)
	// e.g. get /foo/bar/baz.txt would return the foo location & "/bar/baz.txt" if /foo doesn't have a /bar sublocation
	// what happens if we hit a redirection?
	// on redirection: respond with RETURN_CODE as status code
	// and in a Location header, put the new URI
	// most likely this will be <redirect_uri>/<rest>/<of>/<path>/<to>.<match>
	// in any case, if the router hits a location with "return" in it, it immediately returns that location + the remaining path as string
	// then in RequestProcessor::process(), we simply check if the location has a return; if it does, send redirect response with location.returnPath + routerReturnedPair.second.

} // namespace core
