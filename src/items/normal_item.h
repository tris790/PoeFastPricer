#ifndef normal_item_h
#define normal_item_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "base_item.h"

namespace items
{
	class normal_item : public base_item
	{
	public:
		normal_item(
			std::string item_name
		)
		{
			name = item_name;
			rarity = "Normal";
		}

		json to_json();
	};
} // namespace items
#endif
