#ifndef item_parser_h
#define item_parser_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace items
{
	struct unique_item
	{
		std::string type;
		std::string name;
	};

	unique_item get_item_from_clipboard();
} // namespace poerequest
#endif