#include "overview.h"

void poerequest::to_json(json &j, const overview &p)
{
	j = json{{"result", p.result}, {"id", p.id}, {"total", p.total}};
}

void poerequest::from_json(const json &j, overview &p)
{
	j.at("result").get_to(p.result);
	j.at("id").get_to(p.id);
	j.at("total").get_to(p.total);
}