#include "perf.h"
#include <iomanip>

long get_time_since_last_perf()
{
    long span = std::time(nullptr) - last_time;
    last_time = std::time(nullptr);
    return span;
}

void print_time_since_last_perf(std::string log)
{
    std::cout << log << std::setw(40 - log.length()) << "[" << get_time_since_last_perf() << "] elapsed" << std::endl;
}