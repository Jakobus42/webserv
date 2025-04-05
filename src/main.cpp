#include "http/RequestParser.hpp"
#include "http/ResponseParser.hpp"
#include "http/http.hpp"
#include "shared/stringUtils.hpp"

int main() {
	size_t foo = shared::string::toNum<size_t>("1234", std::hex);
	std::cout << foo << std::endl;
}