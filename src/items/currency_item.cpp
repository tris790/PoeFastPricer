#include "currency_item.h"

json items::currency_item::to_json()
{
	return {
		{"query",
			{
				{"status", {{"option", "online"}}},
				{"type", name},
				{"stats",
					{
						{{"type", "and"}, {"filters", json::array({})}, {"disabled", false}}
					}
				}
			}
		},
		{"sort",
		 {{"price", "asc"}}}
	};
}
