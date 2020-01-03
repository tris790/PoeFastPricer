#ifndef currency_item_h
#define currency_item_h
#include <nlohmann/json.hpp>
#include "base_item.h"

using json = nlohmann::json;

namespace items
{
	class currency_item : public base_item
	{
	public:
		std::string quantity;

		currency_item(std::string item_name, std::string item_quantity)
		: quantity(item_quantity)
		{
			name = item_name;
			rarity = "Currency";
		}

		json to_json();
	};

} // namespace items
#endif