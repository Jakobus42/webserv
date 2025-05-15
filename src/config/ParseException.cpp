#include "config/ParseException.hpp"

#include "shared/stringUtils.hpp"

namespace config {

	ParseException::ParseException(const std::string& message)
		: std::runtime_error("Parsing failed: " + message)
		, m_message("Parsing failed: " + message) {}

	ParseException::ParseException(std::size_t line, const std::string& message)
		: std::runtime_error("Line " + shared::string::toString(line) + ": Parsing failed: " + message)
		, m_message("Line " + shared::string::toString(line) + ": Parsing failed: " + message) {}

	ParseException::~ParseException() throw() {}

	const std::string& ParseException::getMessage() const { return m_message; }


} /* namespace config */
