#pragma once

#include <map>
#include <vector>

template<typename A, typename B> std::vector<A> get_keys(std::map<A, B> map)
{
	std::vector<A> keys = std::vector<A>();
	typename std::map<A, B>::iterator i;
	for(i = map.begin(); i != map.end(); i++)
	{
		keys.push_back(i->first);
	}
	return keys;
}

template<typename A, typename B> std::vector<A> get_values(std::map<A, B> map)
{
	std::vector<A> values = std::vector<A>();
	typename std::map<A, B>::iterator i;
	for(i = map.begin(); i != map.end(); i++)
	{
		values.push_back(i->second);
	}
	return values;
}

template<typename A, typename B> A key_lookup(std::map<A, B>& map, B value)
{
	typename std::map<A, B>::iterator i;
	for(i = map.begin(); i != map.end(); i++)
	{
		if(i->second == value)
		{
			return i->first;
		}
	}
}
