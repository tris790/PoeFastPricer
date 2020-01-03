//http://api.pathofexile.com/leagues

#include "leagues.h"

void poerequest::to_json(json& j, const league &p)
{
	j = json{
		{"id", p.id},
		{"realm", p.realm},
		{"description", p.description}
	};
}

void poerequest::from_json(const json& j, league& p)
{
	j.at("id").get_to(p.id);
	j.at("realm").get_to(p.realm);
	j.at("description").get_to(p.description);
}

void poerequest::to_json(json& j, const leagues& p)
{
	j = json{
		{ p.current_leagues }
	};
}

void poerequest::from_json(const json& j, leagues& p)
{
	j.get_to(p.current_leagues);
}