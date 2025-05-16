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
		, m_cgiProcessor(dispatcher, m_serverConfig)
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

	void RequestProcessor::generateErrorResponse(http::StatusCode code) {
		std::string response = "";
		if (!m_router.getResult().empty()) {
			const std::map<http::StatusCode, std::string>& errorPages = m_router.getResult().location->errorPages;
			std::map<http::StatusCode, std::string>::const_iterator it = errorPages.find(code);
			if (it != errorPages.end()) {
				std::string path = m_router.getResult().location->root + "/" + it->second;
				std::string buffer;
				std::ifstream f(path.c_str());
				if (f.is_open()) {
					while (getline(f, buffer)) {
						response += buffer;
					}
					f.close();
					if (f.bad()) {
						response.clear();
					}
				} else {
					LOG_ERROR("generateErrorResponse: open() failed, falling back to default error page");
				}
			}
		}
		if (response == "") {
			response = generateErrorPage(code);
		}
		m_response->setHeader("Connection", "close");
		m_response->appendBody(response);
		m_response->setHeader("Content-Type", "text/html");
	}

	void RequestProcessor::generateRedirectResponse() {
		const Route& route = m_router.getResult();

		m_response->appendHeader("Location", route.redirectUri);
		m_response->setStatusCode(route.returnClass);
	}

	std::string RequestProcessor::generateErrorPage(http::StatusCode code) {
		static const std::string preset = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" />"
										  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>"
										  "<title>error_code</title><style>body {background-color: #f8f8f8;font-family: Arial, sans-serif;"
										  "text-align: center;padding: 50px;color: #333;}.container {display: inline-block;padding: 40px;"
										  "background-color: white;border: 1px solid #ddd;border-radius: 10px;box-shadow: 0 4px 8px rgba(0,0,0,0.05);}"
										  "h1 {font-size: 48px;color: #e74c3c;margin-bottom: 10px;}p {font-size: 18px;color: #666;}</style></head>"
										  "<body><div class=\"container\"><h1>error_code</h1><p>error_text</p></div></body></html>";

		std::string new_error_page = preset;
		std::string error_text = http::statusCodeToMessage(code);
		std::ostringstream oss;
		oss << static_cast<int>(code);
		std::string error_code_str = oss.str();

		size_t pos = 0;
		while ((pos = new_error_page.find("error_code", pos)) != std::string::npos) {
			new_error_page.replace(pos, 10, error_code_str);
			pos += error_code_str.length();
		}
		pos = 0;
		while ((pos = new_error_page.find("error_text", pos)) != std::string::npos) {
			new_error_page.replace(pos, 10, error_text);
			pos += error_text.length();
		}
		return new_error_page;
	}

} // namespace core
