#include <fstream>
#include <string>
#include <boost/multiprecision/mpfr.hpp> // for `mpfr_float`

#include "memory/memory.hpp"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#elif __linux__
#include <unistd.h>
#endif

size_t get_available_memory(void)
{
#ifdef _WIN32
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status); // Get the memory status
	return status.ullAvailPhys; // Return the available physical memory
#elif __APPLE__
	vm_statistics64_data_t vm_stats;
	mach_msg_type_number_t info_count = HOST_VM_INFO64_COUNT;
	host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vm_stats, &info_count); // Get the virtual memory statistics
	return vm_stats.free_count * sysconf(_SC_PAGESIZE); // Return the available memory
#elif __linux__
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
#else
	return 0;
#endif
}

void set_dynamic_precision(void)
{
	size_t available_memory = get_available_memory();
	size_t precision_bits = available_memory != 0 ? available_memory / 8 : 256; // Rough estimate of the precision: 8 bits per byte
	size_t max_precision_bits = 4096; // Maximum precision in bits

	if (precision_bits > max_precision_bits)
	{
		precision_bits = max_precision_bits;
	}

	boost::multiprecision::mpfr_float::default_precision(precision_bits);
}