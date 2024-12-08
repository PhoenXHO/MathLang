#pragma once

#include <chrono>


class Benchmark
{
	std::chrono::high_resolution_clock::time_point start, end;

public:
	Benchmark() = default;
	~Benchmark() = default;

	void start_benchmark(void);
	void end_benchmark(void);
	void print_benchmark_result(void);
};