#include "unique_item.h"

json items::unique_item::to_json()
{
	return {
		{"query",
			{
				{"status", {{"option", "online"}}},
				{"name", name},
				{"type", type},
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
