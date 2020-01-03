#include "normal_item.h"

json items::normal_item::to_json()
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
