#pragma once
#include "cms_collections.h"

namespace cms{



	std::vector<Vec3f> generate_sequence_vec3f(std::function<float()> gen, int count){
		std::vector<Vec3f> res;
		for (auto i : int_range(0, count)){
			res.push_back({gen(), gen(), gen()});
		}
		return res;
	}

	std::vector<float> generate_sequence_f(std::function<float()> gen, int count){
		std::vector<float> res;
		for (auto i : int_range(0, count)){
			res.push_back(gen());
		}
		return res;
	}
}