#ifndef hotkeys_h
#define hotkeys_h
#include "../items/base_item.h"
#include "../request.h"
namespace hotkeys
{
	typedef void (*callback_function)(items::base_item* item, poerequest::result_listing* listings);

	void register_hotkeys();
	void run_price_check(hotkeys::callback_function callback);
} // namespace hotkeys
#endif