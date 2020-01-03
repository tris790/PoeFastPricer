#ifndef item_parser_h
#define item_parser_h
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "items/base_item.h"

namespace items
{
	base_item* get_item_from_clipboard();
} // namespace poerequest
#endif