#pragma once

#include "config/Parser.hpp"
#include "http/ARequestHandler.hpp"
#include "http/http.hpp"
#include "shared/NonCopyable.hpp"

#include <deque>
#include <http/RequestParser.hpp>
#include <map>

namespace http {

	/**
	 * @class RequestProccesor
	 * @brief ...
	 */
	class RequestProccesor : shared::NonCopyable {
		public:
			RequestProccesor(std::vector<config::Location> locations);
			~RequestProccesor();

			Response* process(Request& req);



			// for testing purposes
			void printLocation(const config::Location& location, int detailed);
			int testParseURI(const std::string& uri, int mode);

		private:
			Response* releaseResponse();

		private:
			Response* m_res;
			std::map<Method, ARequestHandler*> m_handlers;
			std::vector<config::Location> m_locations;

			int findLocation(const std::string& uri, const std::vector<config::Location>& locations, config::Location& location);

			bool checkRequestData(Request& request);
			bool checkAndReconstructTargetUri(Request& request);
			bool parseAbsoluteForm(const std::string& path, Request& request);
			bool parseOriginForm(const std::string& path, Request& request);
	};

} /* namespace http */
