#ifndef leagues_h
#define leagues_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace poerequest
{

struct league
{
    std::string id;
    std::string realm;
    std::string description;
};

struct leagues
{
    std::vector<league> current_leagues;
};

void to_json(json &j, const league &p);
void to_json(json &j, const leagues &p);


void from_json(const json &j, league &p);
void from_json(const json &j, leagues &p);
} // namespace poerequest
#endif