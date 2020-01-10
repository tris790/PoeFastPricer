#include "hotkeys.h"
#include <Windows.h>
#include "../perf.h"

void hotkeys::register_hotkeys()
{
	RegisterHotKey(
		NULL,
		1,
		MOD_CONTROL,
		0x44);
}

void hotkeys::run_price_check(hotkeys::callback_function callback)
{
	std::cout << "--------------------------------------------------" << std::endl;
	print_time_since_last_perf("Hotkey triggered");
	HWND handle = GetForegroundWindow();
	std::string poe = "Path of Exile";
	int window_title_len = GetWindowTextLength(handle);
	int new_len = window_title_len + 1;
	char* window_title = new char[new_len];
	GetWindowText(handle, window_title, poe.length() + 1);

	bool isPathOfExile = poe == std::string(window_title);
	if (isPathOfExile)
	{
		items::base_item* item = items::get_item_from_clipboard();
		if (!item)
		{
			return;
		}

		print_time_since_last_perf("Pricing: " + item->name);

		poerequest::league league;
		get_league(true, true, league);

		poerequest::overview overview;
		print_time_since_last_perf("Sending get leagues request");
		get_overview(league.id, item, overview);

		poerequest::result_listing listings;
		get_price_listings(overview, listings);
		auto low_price = listings.result.front().listing.price;
		auto high_price = listings.result.back().listing.price;

		std::cout << "Ranging from [" << low_price.amount << " " << low_price.currency
			<< " - " << high_price.amount << " " << high_price.currency << "]" << std::endl;

		callback(item, &listings);
		delete item;
	}
}