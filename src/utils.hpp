#pragma once

#include <optional>
#include <string>

#include "ex_registry.hpp"

std::optional<std::string> read_figure(uint16_t chapter, std::string figure, element_type which);
std::string read_file_or_resource(std::string path);
std::string read_default_os();
void write_errors(void* errors, void* error_file);