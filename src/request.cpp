#include <string>
#include "requestType/overview.h"
#include "requestType/result_listing.h"
#include <iostream>
#include <curl\curl.h>
#include <sstream>
#include "item_parser.h"
#include "request.h"
#include "perf.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void get_overview(const std::string league, items::base_item* item, poerequest::overview &overview)
{
	std::string url = "https://www.pathofexile.com/api/trade/search/" + league;
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		std::string readBuffer;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		std::string js_dump = item->to_json().dump();
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, js_dump.c_str());

		res = curl_easy_perform(curl);
		print_time_since_last_perf("[POST] Sending Request For Overview");

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

		curl_easy_cleanup(curl);

		json json_overview = json::parse(readBuffer);
		overview = json_overview.get<poerequest::overview>();
		print_time_since_last_perf("[JSON] Parsing the Overview Request");
	}
}

void get_price_listings(const poerequest::overview& overview, poerequest::result_listing& listings)
{
	std::ostringstream ss;
	ss << "https://www.pathofexile.com/api/trade/fetch/";
	bool first = true;
	for (auto ptr = overview.result.begin(); ptr < overview.result.begin() + 10; ptr++)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			ss << ',';
		}
		ss << *ptr;
	}
	ss << "?query=" + overview.id;

	std::string url = ss.str();
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		std::string readBuffer;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		struct curl_slist *headers = NULL;

		res = curl_easy_perform(curl);
		print_time_since_last_perf("[GET] Sending the Listing Request");

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

		curl_easy_cleanup(curl);

		json json_listings = json::parse(readBuffer);
		listings = json_listings.get<poerequest::result_listing>();
		print_time_since_last_perf("[JSON] Parsing the Listings Request");
	}
}

void get_league(const bool isSoftcore, const bool isChallengeLeague, poerequest::league& league)
{
	std::string url = "http://api.pathofexile.com/leagues";
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		std::string readBuffer;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		struct curl_slist* headers = NULL;

		res = curl_easy_perform(curl);
		print_time_since_last_perf("[GET] Sending the Get League Request");

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		curl_easy_cleanup(curl);
		json json_leagues = json::parse(readBuffer);
		int leagueIndex = (isSoftcore ? 0 : 1) + (isChallengeLeague ? 4 : 0);
		league = json_leagues.get<poerequest::leagues>().current_leagues.at(leagueIndex);
		print_time_since_last_perf("[JSON] Parsing the Get League Request");
	}
}
