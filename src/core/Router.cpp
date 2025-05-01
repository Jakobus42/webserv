#include "core/Router.hpp"

namespace core {

	Router::Router()
		: m_routeResult() {}

	Router::~Router() {}

	void Router::route(const shared::string::StringView& uriPath, const config::LocationConfig& currentLocation, std::size_t depth) {
		if (currentLocation.hasRedirect()) {
			m_routeResult.remainingPath = uriPath.toString();
			m_routeResult.location = &currentLocation;
			m_routeResult.generateRedirectUri();
			return;
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
		m_routeResult.location = &currentLocation;
		m_routeResult.remainingPath = uriPath.toString();
		m_routeResult.generateAbsoluteFilePath();
	}

	bool Router::methodIsAllowed(http::Method method) const {
		const std::set<http::Method>& allowedMethods = m_routeResult.location->allowedMethods;
		if (allowedMethods.find(method) == allowedMethods.end()) {
			return false;
		}
		return true;
	}

	void Router::reset() { m_routeResult.reset(); }

	const Route& Router::getResult() const { return m_routeResult; }

	// ------------------------  Route class implementation  ---------

	Route::Route()
		: location(NULL)
		, remainingPath("")
		, absoluteFilePath("")
		, redirectUri("") {}

	Route::~Route() {}

	Route::Route(const Route& other)
		: location(other.location)
		, remainingPath(other.remainingPath)
		, absoluteFilePath(other.absoluteFilePath)
		, redirectUri(other.redirectUri) {}

	Route& Route::operator=(const Route& rhs) {
		if (this != &rhs) {
			location = rhs.location;
			remainingPath = rhs.remainingPath;
			absoluteFilePath = rhs.absoluteFilePath;
			redirectUri = rhs.redirectUri;
		}
		return *this;
	}

	bool Route::empty() const { return location == NULL; }

	void Route::reset() {
		location = NULL;
		remainingPath.clear();
		absoluteFilePath.clear();
		redirectUri.clear();
	}

	void Route::generateAbsoluteFilePath() { absoluteFilePath = location->precalculatedAbsolutePath + remainingPath; }

	void Route::generateRedirectUri() { redirectUri = location->redirectUri + remainingPath; }

} /* namespace core */
