#pragma once
#include "config/LocationConfig.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <string>

namespace core {
	struct Route {
		public:
			Route();
			~Route();
			Route(const Route& other);
			Route& operator=(const Route& rhs);

			bool empty() const;
			void reset();

			void generateFilePath();
			void generateRedirectUri();

		public:
			const config::LocationConfig* location;
			std::string remainingPath;
			std::string filePath;
			std::string redirectUri;
			http::StatusCode returnClass;
	};

	class Router : public shared::mixin::NonCopyable {
		public:
			Router();
			~Router();

		public:
			void route(const shared::string::StringView& uriPath, const config::LocationConfig& currentLocation, std::size_t depth = 0);
			void reset();

			bool methodIsAllowed(http::Method method) const;
			const Route& getResult() const;

		private:
			Route m_routeResult;
			static const std::size_t MAX_ROUTE_DEPTH = 8;
	};

} /* namespace core */
