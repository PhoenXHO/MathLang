#include <iostream>

#include "util/benchmark.hpp"
#include "util/config.hpp"

void Benchmark::start_benchmark(void)
{
	if (!config::benchmark)
		return;
	start = std::chrono::high_resolution_clock::now();
}
void Benchmark::end_benchmark(void)
{
	if (!config::benchmark)
		return;
	end = std::chrono::high_resolution_clock::now();
}
void Benchmark::print_benchmark_result(void)
{
	if (!config::benchmark)
		return;
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	//std::cout << "(Executed in " << duration.count() << " microseconds)\n";
	// Print in italic
	std::cout << "\033[3m(Executed in " << duration.count() << " microseconds)\033[0m\n";
}