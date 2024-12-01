#include <fstream>
#include <string>
#include <boost/multiprecision/mpfr.hpp> // for `mpfr_float`

#include "memory/memory.hpp"

size_t get_available_memory(void)
{
	std::ifstream meminfo("/proc/meminfo");
	std::string line;
	size_t available_memory = 0;

	while (std::getline(meminfo, line))
	{
		if (line.find("MemAvailable") != std::string::npos)
		{
			std::istringstream iss(line);
			std::string mem;
			size_t value;
			std::string unit;

			// Extract the available memory in KiB (it will be in the form "MemAvailable: <value> kB")
			iss >> mem >> value >> unit;
			available_memory = value * 1024; // Convert from KiB to bytes
			break;
		}
	}

	return available_memory;
}

void set_dynamic_precision(void)
{
	size_t available_memory = get_available_memory();
	size_t max_precision = std::log(available_memory) * 8;

	// Estimate the number of bits we can use for the precision
	// Assuming each bit of precision requires 1 byte of memory
	size_t precision_bits = max_precision * 8 > 512 ? 512 : max_precision * 8; // 512 bits is the maximum precision we can use

	boost::multiprecision::mpfr_float::default_precision(precision_bits);
}