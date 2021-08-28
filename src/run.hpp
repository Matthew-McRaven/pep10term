#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

void save_runtime_output(const std::vector<uint8_t> output, std::filesystem::path output_path);