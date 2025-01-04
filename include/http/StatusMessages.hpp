#pragma once

#include "shared/defines.hpp"

#include <map>
#include <string>

namespace http {

	typedef std::pair<http::StatusCode, std::string> t_statusMessage;
	typedef std::map<t_statusMessage::first_type, t_statusMessage::second_type> t_statusMessages;

	/**
	 * @class StatusMessages
	 * @brief Singleton class to store and retrieve status messages
	 */
	class StatusMessages {
		public:
			~StatusMessages();

			static const StatusMessages& getInstance() {
				static StatusMessages instance;
				return instance;
			}

			// singleton is otherwise lazily initialized
			static void eagerInit() {
				(void)getInstance();
			}

			const t_statusMessages& getStatusMessages() const;

		private:
			StatusMessages();
			StatusMessages(const StatusMessages& other);
			StatusMessages& operator=(const StatusMessages& rhs);

			t_statusMessages m_statusMessages;
	};

} /* namespace http */
