#ifndef base_item_h
#define base_item_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace items
{
	class base_item
	{
	public:
		std::string name;
		std::string rarity;

		virtual json to_json() = 0;
	};

} // namespace items
#endif