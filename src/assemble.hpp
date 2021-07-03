#pragma once

#include <filesystem>
#include <string>
#include <vector>


void save_listing(std::string listing, std::filesystem::path listing_path);
void save_object_code(std::string bytes, std::filesystem::path object_path);