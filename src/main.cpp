#include <curl/curl.h>
#include <iostream>
#include <sstream>

#include "requestType/overview.h"
#include "requestType/result_listing.h"
#include "request.h"
#include "item_parser.h"
#include "hotkeys/hotkeys.h"
#include "gui/gui.h"


int main()
{
	hotkeys::register_hotkeys();

	curl_global_init(CURL_GLOBAL_ALL);
	std::cout << "Poe Fast Pricer" << std::endl;

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			hotkeys::run_price_check([](items::base_item* item, poerequest::result_listing* listings) {
				run_display(item, listings);
			});
		}
	}

	curl_global_cleanup();
	return 0;
}