#pragma once
#include "config/LocationConfig.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <string>

namespace core {

	class Router : public shared::mixin::NonCopyable {
		private:
			struct Route : shared::mixin::NonCopyable {
				public:
					Route();
					~Route();

					bool empty() const;
					void reset();

				public:
					const config::LocationConfig* location;
					std::string filePath;
			};

		public:
			Router();
			~Router();

		public:
			void route(const shared::string::StringView& uriPath, const config::LocationConfig& currentLocation, std::size_t depth = 0);
			void reset();

			bool needsRoute() const;
			bool foundRedirect() const;
			const Route& getResult() const;
			std::string generateRedirectUri() const;
			http::StatusCode getReturnClass() const;

		private:
			Route m_routeResult;
			static const std::size_t MAX_ROUTE_DEPTH = 8;
	};

} /* namespace core */
