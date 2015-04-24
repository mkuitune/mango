#pragma once
#include "cms_math.h"
#include <functional>
#include <vector>

namespace cms{
	struct int_range{
		int startvalue, endvalue;
		struct iterator{
			int v; 
			iterator(int i) :v(i){}
			void operator++(){ v++; }
			int& operator*(){ return v; }
			bool operator!=(const iterator& rhs){ return v != rhs.v; }
		};
		int_range(int s, int e) :startvalue(s), endvalue(e){}
		iterator begin(){ return iterator(startvalue); }
		iterator end(){ return iterator(endvalue); }
	};

	std::vector<Vec3f> generate_sequence_vec3f(std::function<float()> gen, int count);
	std::vector<float> generate_sequence_f(std::function<float()> gen, int count);
}