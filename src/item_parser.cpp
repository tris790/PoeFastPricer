#include "item_parser.h"
#include "clipboard.h"
#include <sstream>

items::unique_item items::get_item_from_clipboard()
{
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

	items::unique_item item;
	item.name = lines.at(1);
	item.type = lines.at(2);
	return item;
}
