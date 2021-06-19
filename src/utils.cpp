#include "utils.hpp"

#include <fstream>
#include <regex>


#include "ex_registry.hpp"

std::optional<std::string> read_figure(uint16_t chapter, std::string figure, element_type type)
{
	static const auto reg = registry::instance();
	auto fig = reg.find("pep10", chapter, figure);
	if(!fig) return std::nullopt;
	if(auto it = fig->elements.find(type); it == fig->elements.end()) return std::nullopt;
	else return it->second;

}
std::string read_file_or_resource(std::string path)
{
	static const auto rc_reg = std::regex("rc://.*");
	if(std::regex_match(path, rc_reg)) {
		throw std::logic_error("Loading a resource is not yet implemented!");
	}
	else {
		std::ifstream file_source(path);
		// Extra () are necessary! Otherwise it won't work.
		// See: https://stackoverflow.com/a/2912614
		return std::string( (std::istreambuf_iterator<char>(file_source) ),(std::istreambuf_iterator<char>()));
	}
}

std::string read_default_os()
{
	throw std::logic_error("Not yet implemented");
}

void write_errors(void* errors, void* error_file)
{
	throw std::logic_error("Not yet implemented");
}