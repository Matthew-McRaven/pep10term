#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <regex>

#include <boost/algorithm/string.hpp>
#include "fmt/core.h"

#include "ex_registry.hpp"

std::vector<std::string> split(const std::string& input, const std::string& regex) {
	std::vector<std::string> ret;
	boost::split(ret, input ,boost::is_any_of("."));
	return ret;
}

std::optional<std::tuple<std::string, int, std::string>> parse_file_as_resource(const std::string& path) {
	static const auto rc_reg = std::regex("rc://.*");
	if(std::regex_match(path, rc_reg)) {
		auto without_prefix = path.substr(5, path.size());
		auto substrs = split(without_prefix, "\\.");
		return {{substrs[0], std::stoi(substrs[1]), substrs[2]}};
	}
	else return std::nullopt;
}

bool is_resource(const std::string& path)
{
	return static_cast<bool>(parse_file_as_resource(path));
}

std::optional<std::string> read_macro(const std::string& macro)
{
	static const auto reg = registry::instance();
	auto fig = reg.find(macro);
	if(!fig) return std::nullopt;
	else return fig->text;

}

std::optional<std::string> read_figure(uint16_t chapter, const std::string& figure, element_type type)
{
	static const auto reg = registry::instance();
	auto fig = reg.find("pep10", chapter, figure);
	if(!fig) return std::nullopt;
	if(auto it = fig->elements.find(type); it == fig->elements.end()) return std::nullopt;
	else return it->second;

}

std::string read_file_or_resource(const std::string& path)
{
	if(auto as_rc = parse_file_as_resource(path); as_rc) {
		auto& [proc, ch, fig] = *as_rc;
		std::cout << fig << std::endl;
		auto book_fig = read_figure(ch, fig, element_type::kPep);
		if(book_fig) return *book_fig;
		else throw std::logic_error(fmt::format("No such Figure rc://{}/{}.{}", proc, ch, fig));
	}
	else {
		std::ifstream file_source(path);
		// Extra () are necessary! Otherwise it won't work.
		// See: https://stackoverflow.com/a/2912614
		return std::string( (std::istreambuf_iterator<char>(file_source)),(std::istreambuf_iterator<char>()));
	}
}

std::string read_default_os()
{
	return *read_figure(9, "00", element_type::kPep);
}

void write_errors(void* errors, void* error_file)
{
	throw std::logic_error("Not yet implemented");
}