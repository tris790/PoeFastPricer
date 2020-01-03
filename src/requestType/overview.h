#ifndef overview_h
#define overview_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace poerequest
{
struct overview
{
    std::vector<std::string> result;
    std::string id;
    int total;
};

void to_json(json &j, const overview &p);

void from_json(const json &j, overview &p);
} // namespace poerequest
#endif