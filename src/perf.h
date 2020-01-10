#ifndef perf_h
#define perf_h
#include <ctime>
#include <iostream>
static std::time_t last_time = std::time(nullptr);
long get_time_since_last_perf();
void print_time_since_last_perf(std::string log);
#endif