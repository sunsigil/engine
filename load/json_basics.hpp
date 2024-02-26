#pragma once

#include "formats/json.h"
#include <iostream>
#include <glm/glm.hpp>

template<int N> glm::vec<N, float> vec_JSON_init(JSON_head_t* json)
{
	if(*json != JSON_ARRAY)
	{
		std::cerr << "[vec_JSON_init] error: expected JSON_ARRAY vec representation" << std::endl;
		return glm::vec<N, float>(0);
	}

	glm::vec<N, float> vec;
	for(int i = 0; i < N; i++)
	{
		JSON_POD_t* pod_i = (JSON_POD_t*) JSON_array_get(json, i);
		vec[i] = pod_i->value.float_value;
	}
	
	return vec;
}

