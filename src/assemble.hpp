#pragma once

#include <filesystem>
#include <string>
#include <vector>


void save_listing(std::string listing, void* source_path);
void save_object_code(std::vector<uint8_t> bytes, std::filesystem::path object_path);