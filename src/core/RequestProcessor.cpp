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
			http::Request::Type requestType = request.getType();

			if (requestType == http::Request::FETCH) {
				if (m_router.needsRoute()) {
					m_router.route(shared::string::StringView(request.getUri().getPath().c_str()), m_serverConfig.location);
					if (m_router.foundRedirect()) {
						generateRedirectResponse();
						return false;
					}
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

	void RequestProcessor::generateErrorResponse(http::StatusCode code) {
		std::string response = "";
		if (!m_router.needsRoute()) {
			const std::map<http::StatusCode, std::string>& errorPages = m_router.getResult().location->errorPages;
			std::map<http::StatusCode, std::string>::const_iterator it = errorPages.find(code);
			if (it != errorPages.end()) {
				std::string path = m_router.getResult().location->precalculatedAbsolutePath + "/" + it->second;
				std::string buffer;
				std::ifstream f(path.c_str());
				if (f.is_open()) {
					while (getline(f, buffer)) {
						response += buffer;
					}
					f.close();
				}
				// else    //maybe give feedback if an error page is not opened
			}
		}
		if (response == "")
			response = generateErrorPage(code);
		m_response->appendBody(response);
	}

	// todo: add Location header with new uri
	void RequestProcessor::generateRedirectResponse() {
		m_response->appendBody("tmp redirect response\n"); // todo:
		m_response->appendBody(m_router.generateRedirectUri() + "\n");
		m_response->setStatusCode(m_router.getReturnClass());
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
