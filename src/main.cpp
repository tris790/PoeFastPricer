#include <curl/curl.h>
#include <iostream>
#include "requestType/overview.h"
#include "requestType/result_listing.h"
#include "request.h"
#include "item_parser.h"

#include <Windows.h>
using json = nlohmann::json;
HWND Proccess_window;
const std::string League = "Metamorph";

int main()
{
	Proccess_window = GetConsoleWindow(); //GetForegroundWindow();
	if (!RegisterHotKey(
		NULL,
		1,
		MOD_CONTROL,
		0x44))
	{
		std::cout << "Failed to register hotkey " << GetLastError() << std::endl;

		return 1;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	std::cout << "Poe Fast Pricer" << std::endl;

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			HWND handle = GetForegroundWindow();
			std::string poe = "Path of Exile";
			int window_len = GetWindowTextLength(handle);
			char* window_title = new char[window_len + 1];
			GetWindowText(handle, window_title, poe.length() + 1);

			bool isPathOfExile = poe == std::string(window_title);
			if (isPathOfExile)
			{
				items::unique_item item = items::get_item_from_clipboard();
				std::cout << "Pricing: " << item.name << " " << item.type << std::endl;

				poerequest::overview overview;
				get_overview(League, item, overview);

				poerequest::result_listing listings;
				get_price_listings(overview, listings);
				auto low_price = listings.result.front().listing.price;
				auto high_price = listings.result.back().listing.price;
				std::cout << "Ranging from [" << low_price.amount << " " << low_price.currency 
					<< " - " << high_price.amount << " " << high_price.currency << "]" << std::endl;
				SetForegroundWindow(Proccess_window);
			}
		}
	}
	curl_global_cleanup();
	return 0;
}