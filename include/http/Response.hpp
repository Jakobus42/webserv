#pragma once

#include "http/AMessage.hpp"
#include "http/http.hpp"

namespace http {

	class Response : public AMessage {
		public:
			Response();
			~Response();

			const std::string& serialize();

			/* Start Line */
			const std::string& getReasonPhrase() const;

			void setReasonPhrase(const std::string& phrase);
			void setReasonPhrase(const shared::string::StringView& phrase);

		private:
			std::string m_reasonPhrase;

			std::string m_serializedData;
	};

} /* namespace http */
