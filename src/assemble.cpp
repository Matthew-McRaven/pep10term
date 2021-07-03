#include "assemble.hpp"


#include <fstream>

#include <asmb/pep10/create_driver.hpp>
#include <masm/utils/listing.hpp>

void save_listing(std::string listing, std::filesystem::path listing_path)
{
	std::ofstream file(listing_path);
    file << listing;
    file.close();
}

void save_object_code(std::string bytes, std::filesystem::path object_path)
{
	std::ofstream file(object_path);
    file << bytes;
    file.close();
}