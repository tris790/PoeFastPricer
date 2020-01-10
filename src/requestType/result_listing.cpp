#include "result_listing.h"

void poerequest::to_json(json& j, const result_listing& p)
{
	j = json{
		{"result", p.result} };
}

void poerequest::from_json(const json& j, result_listing& p)
{
	j.at("result").get_to(p.result);
}

void poerequest::to_json(json& j, const result_t& p)
{
	j = json{
		{"id", p.id},
		{"listing", p.listing} };
}

void poerequest::from_json(const json& j, result_t& p)
{
	j.at("id").get_to(p.id);
	j.at("listing").get_to(p.listing);
}

void poerequest::to_json(json& j, const listing_t& p)
{
	j = json{
		{"indexed", p.indexed},
		{"price", p.price},
		{"account", p.account} };
}

void poerequest::from_json(const json& j, listing_t& p)
{
	j.at("indexed").get_to(p.indexed);
	j.at("price").get_to(p.price);
	j.at("account").get_to(p.account);
}

void poerequest::to_json(json& j, const price_t& p)
{
	j = json{
		{"currency", p.currency},
		{"amount", p.amount} };
}

void poerequest::from_json(const json& j, price_t& p)
{
	j.at("currency").get_to(p.currency);
	j.at("amount").get_to(p.amount);
}

void poerequest::to_json(json& j, const account_t& p)
{
	j = json{
		{"name", p.name} };
}

void poerequest::from_json(const json& j, account_t& p)
{
	j.at("name").get_to(p.name);
}