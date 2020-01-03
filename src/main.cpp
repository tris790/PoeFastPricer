#include <curl/curl.h>
#include <iostream>
#include "requestType/overview.h"
#include "requestType/result_listing.h"
#include "request.h"
#include "item_parser.h"
// for convenience
using json = nlohmann::json;

int main()
{
	curl_global_init(CURL_GLOBAL_ALL);
	std::cout << "Poe Fast Pricer" << std::endl;

	std::string league = "Metamorph";
	items::unique_item item = items::get_item_from_clipboard();
	std::cout << item.name << " " << item.type << std::endl;

	poerequest::overview overview;
	get_overview(league, item, overview);
	std::cout << "post " << overview.id << std::endl;

	poerequest::result_listing listings;
	get_price_listings(overview, listings);
	auto price = listings.result.front().listing.price;
	std::cout << "get " << price.amount << " " << price.currency << std::endl;

	curl_global_cleanup();
	return 0;
}