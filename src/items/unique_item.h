#ifndef unique_item_h
#define unique_item_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "base_item.h"

namespace items
{
	class unique_item : public base_item
	{
	public:
		std::string type;
		std::string ilvl;
		std::vector<std::string> implicits;
		std::vector<std::string> stats;

		unique_item(
			std::string item_name,
			std::string item_type,
			std::string item_ilvl,
			std::vector<std::string> item_implicit,
			std::vector<std::string> item_stats
		)
			: type(item_type), ilvl(item_ilvl), implicits(item_implicit), stats(item_stats)
		{
			name = item_name;
			rarity = "Unique";
		}

		json to_json();
	};
} // namespace items
#endif
