#pragma once

#include "config/Parser.hpp"
#include "http/ARequestHandler.hpp"
#include "http/http.hpp"
#include "shared/NonCopyable.hpp"
#include "config/Parser.hpp"

#include <deque>
#include <http/RequestParser.hpp>
#include <map>

namespace http {

	typedef struct s_PathData {
		std::string scheme;
		std::string authority;
		std::string pure_path;
		std::string query;
	} t_PathData;

	/**
	 * @class RequestProccesor
	 * @brief ...
	 */
	class RequestProccesor : shared::NonCopyable {
		public:
			RequestProccesor(std::vector<config::t_location> locations);
			~RequestProccesor();

			Response* process(const Request& req);



			// for testing purposes
			void printLocation(const config::t_location location, int detailed);
			int testParseURI(const std::string& uri, int mode);
		private:
			Response* releaseResponse();

		private:
			std::map<Method, ARequestHandler*> m_handlers;
			Response* m_res;
			std::vector<config::t_location> m_locations;
			t_PathData m_pathData;
			http::StatusCode m_statusCode;

			int findLocation(const std::string& uri, const std::vector<config::t_location>& locations, config::t_location& location);
			
			bool checkRequestData(const Request &request);
			bool checkAndReconstructTargetUri(const Request &request);
			bool parseAbsoluteForm(const std::string &path, const Request& request, t_PathData &pathData);
			bool parseOriginForm(const std::string &path, const Request& request, t_PathData &pathData);
	};

} /* namespace http */
