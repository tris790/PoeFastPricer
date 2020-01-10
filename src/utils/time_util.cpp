#include <string>
#include <time.h>
#include <ctime>

std::string time_since(std::string utc_date)
{
    int y, M, d, h, m;
    float s;
    (void)sscanf(utc_date.c_str(), "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &s);

    tm time;
    time.tm_year = y - 1900; // Year since 1900
    time.tm_mon = M - 1;     // 0-11
    time.tm_mday = d;        // 1-31
    time.tm_hour = h;        // 0-23
    time.tm_min = m;         // 0-59
    time.tm_sec = (int)s;

    time_t indexed = std::mktime(&time);
    std::time_t now_t = std::time(0);
    std::tm *now_tm = std::gmtime(&now_t);
    time_t now = std::mktime(now_tm);
    auto span = now - indexed;
    if (span > 60 * 60 * 24) // Days
    {
        return std::to_string(span / (60 * 60 * 24)) + " Days";
    }
    else if (span > 60 * 60) // hours
    {
        return std::to_string(span / (60 * 60)) + " Hours";
    }

    else if (span > 60) // Minutes
    {
        return std::to_string(span / 60) + " Minutes";
    }
    // Seconds
    return std::to_string(span) + " Seconds";
}