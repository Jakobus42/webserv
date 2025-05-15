#pragma once
#include <stdexcept>
#include <string>

namespace config {

	class ParseException : public std::runtime_error {
		public:
			explicit ParseException(const std::string& message);
			explicit ParseException(std::size_t line, const std::string& message);
			virtual ~ParseException() throw();

			const std::string& getMessage() const;

		private:
			std::string m_message;
	};

} /* namespace config */
