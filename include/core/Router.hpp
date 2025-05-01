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

			const std::string generateFilePath() const;

		public:
			const config::LocationConfig* location;
			std::string filePath;
	};

	class Router : public shared::mixin::NonCopyable {
		public:
			Router();
			~Router();

		public:
			void route(const shared::string::StringView& uriPath, const config::LocationConfig& currentLocation, std::size_t depth = 0);
			void reset();

			bool methodIsAllowed(http::Method method) const;
			bool shouldRedirect() const;
			const Route& getResult() const;
			std::string generateFilePath() const;
			std::string generateRedirectUri() const;
			http::StatusCode getReturnClass() const;

		private:
			Route m_routeResult;
			static const std::size_t MAX_ROUTE_DEPTH = 8;
	};

} /* namespace core */
