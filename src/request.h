#ifndef request_h
#define request_h
#include "requestType/overview.h"
#include "requestType/result_listing.h"
#include "item_parser.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
void get_overview(const std::string league, items::base_item* item, poerequest::overview& overview);
void get_price_listings(const poerequest::overview& overview, poerequest::result_listing& listings);
#endif