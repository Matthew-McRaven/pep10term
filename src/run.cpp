#include "run.hpp"
void save_runtime_output(const std::vector<uint8_t> output, std::filesystem::path output_path)
{
	std::ofstream file(output_path);
	file.write((char*)output.data(), output.size());
	file.close();
}