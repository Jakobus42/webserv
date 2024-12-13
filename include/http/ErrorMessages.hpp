#pragma once

#include "shared/defines.hpp"

#include <map>
#include <string>

namespace http {

	typedef std::pair<http::StatusCode, std::string> t_errorMessage;
	typedef std::map<t_errorMessage::first_type, t_errorMessage::second_type> t_errorMessages;

	/**
	 * @class ErrorMessages
	 * @brief Singleton class to store and retrieve error messages
	 */
	class ErrorMessages {
		public:
			~ErrorMessages();

			static const ErrorMessages &getInstance() {
				static ErrorMessages instance;
				return instance;
			}

			// singleton is otherwise lazily initialized
			static void eagerInit() {
				(void)getInstance();
			}

			const t_errorMessages &getErrorMessages() const;
		private:
			ErrorMessages();
			ErrorMessages(const ErrorMessages &other);
			ErrorMessages &operator=(const ErrorMessages &rhs);

			t_errorMessages m_errorMessages;
	};

} /* namespace http */
