#include "item_parser.h"
#include "clipboard.h"
#include <sstream>

#include "items/unique_item.h"
#include "items/currency_item.h"
#include "items/normal_item.h"

static enum RarityEnum {
	Normal,
	Magic,
	Rare,
	Unique,
	Currency
};

static std::map<std::string, RarityEnum> s_mapStringValues = 
{
	{"Normal", RarityEnum::Normal},
	{"Magic", RarityEnum::Magic},
	{"Rare", RarityEnum::Rare},
	{"Unique", RarityEnum::Unique},
	{"Currency", RarityEnum::Currency}
};
items::base_item* items::get_item_from_clipboard()
{
	std::string backup_clip;
	get_clipboard(backup_clip);

	fake_ctrl_c();
	std::string clip;
	get_clipboard(clip);
	std::vector<std::string> lines;

	char* delim = "\r\n";
	char* token = strtok(const_cast<char*>(clip.c_str()), delim);
	while (token != nullptr)
	{
		lines.push_back(std::string(token));
		token = strtok(nullptr, delim);
	}

	std::string rarity = lines.at(0).substr(std::string("Rarity: ").length());
	std::string name = lines.at(1);

	base_item* item = NULL;
	switch (s_mapStringValues[rarity])
	{
	case RarityEnum::Normal:
	{
		item = new normal_item(name);
		break;
	}
	case RarityEnum::Magic:
	{
		break;
	}
	case RarityEnum::Rare:
	{
		break;
	}
	case RarityEnum::Unique:
	{
		item = new unique_item(name, lines.at(2), "69", {}, {});
		break;
	}
	case RarityEnum::Currency:
	{
		auto quantity = std::find_if(lines.begin(), lines.end(),
			[](std::string line) { return line.find("Stack Size", 0) != std::string::npos; });

		item = new currency_item(name, *quantity);
		break;
	}
	default: {
		throw("Unknown type");

	}
	}

	set_clipboard(backup_clip);
	return item;
}