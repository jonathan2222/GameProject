#pragma once

#include "glm/glm.hpp"
#include <unordered_map>
#include <random>

class Utils
{
public:
	static void init();
	static void destory();

	static glm::vec3 uniformHemisphere();
	static glm::vec3 uniformSphere();
	static glm::vec3 uniformCircle();

	static float random(float lo=0.0f, float hi=1.0f);

	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator () (const std::pair<T1, T2> &p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);

			// Mainly for demonstration purposes, i.e. works but is overly simple
			// In the real world, use sth. like boost.hash_combine
			return h1 ^ h2;
		}
	};

private:
	static std::mt19937* generator;
	static std::unordered_map<std::pair<float, float>, std::uniform_real_distribution<float>*, pair_hash> uniformDistributions;
};