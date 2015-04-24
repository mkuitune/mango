#pragma once

#include <cstdint>
#include <random>
#include <cmath>

namespace cms { 

#define CMS_RAND_SEED 7894321

	// Minimal PRNG of Park & Miller
	inline uint32_t minrandu(uint32_t a) {
		return ((uint64_t) a * 279470273UL) % 4294967291UL;
	}
	inline int32_t minrand(int32_t a) {
		return (int32_t) minrandu(*((uint32_t*) &a));
	}

	struct RandomInt
	{
		std::mt19937 gen;
		std::uniform_int_distribution<> dis;
		RandomInt() :gen(), dis(0, 2147483647){}
		RandomInt(int seed) :gen(seed), dis(0, 2147483647){}
		RandomInt(int start, int end) :gen(), dis(start, end){}
		int32_t rand(){ return dis(gen); }
		int32_t operator()(){ return dis(gen); }
	};

	struct RandomFloat
	{
		std::mt19937 gen;
		std::uniform_real_distribution<float> dis;
		RandomFloat() : gen(), dis(0.0, 1.0){}
		RandomFloat(int seed) : gen(seed), dis(0.0, 1.0){}
		RandomFloat(float start, float end) :gen(), dis(start, end){}
		RandomFloat(float start, float end, int seed) :gen(seed), dis(start, end){}
		float rand(){ return dis(gen); }
		float operator()(){ return dis(gen); }
	};
	
	struct RandomDouble
	{
		std::mt19937 gen;
		std::uniform_real_distribution<double> dis;
		RandomDouble() : gen(), dis(0.0, 1.0){}
		RandomDouble(int seed) : gen(seed), dis(0.0, 1.0){}
		RandomDouble(double start, double end) :gen(), dis(start, end){}
		RandomDouble(double start, double end, int seed) :gen(seed), dis(start, end){}
		double rand(){ return dis(gen); }
		double operator()(){ return dis(gen); }
	};
}