#ifndef result_listing_h
#define result_listing_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace poerequest
{

struct price_t
{
    float amount;
    std::string currency;
};

struct listing_t
{
    std::string indexed;
    price_t price;
};

struct result_t
{
    std::string id;
    listing_t listing;
};

struct result_listing
{
    std::vector<result_t> result;
};

void to_json(json &j, const result_listing &p);

void from_json(const json &j, result_listing &p);

void to_json(json &j, const result_t &p);

void from_json(const json &j, result_t &p);

void to_json(json &j, const listing_t &p);

void from_json(const json &j, listing_t &p);

void to_json(json &j, const price_t &p);

void from_json(const json &j, price_t &p);
} // namespace poerequest
#endif