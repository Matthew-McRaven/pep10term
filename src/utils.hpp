#pragma once

#include <optional>
#include <string>

#include "ex_registry.hpp"

std::optional<std::string> read_macro(const std::string& macro);
std::optional<std::string> read_figure(uint16_t chapter, const std::string& figure, element_type which);

std::optional<std::tuple<std::string, int, std::string>> parse_file_as_resource(const std::string& path);
bool is_resource(const std::string& path);

std::string read_file_or_resource(const std::string& path);
std::string read_default_os();
void write_errors(void* errors, void* error_file);