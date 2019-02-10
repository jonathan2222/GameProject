#include "Utils.h"

#include <ctime>
#include "Logger.h"

std::mt19937* Utils::generator = nullptr;
std::unordered_map<std::pair<float, float>, std::uniform_real_distribution<float>*, Utils::pair_hash> Utils::uniformDistributions = std::unordered_map<std::pair<float, float>, std::uniform_real_distribution<float>*, Utils::pair_hash>();

void Utils::init()
{
	if (generator == nullptr)
	{
		generator = new std::mt19937((unsigned int)time(0));
	}
}

void Utils::destory()
{
	if (generator != nullptr)
	{
		delete generator;
		for (auto& ud : uniformDistributions)
		{
			delete ud.second;
		}
	}
}

glm::vec3 Utils::uniformHemisphere()
{
	float a = random();
	float b = random();
	float sq = sqrtf(1.0f - b * b);
	glm::vec3 v;
	v.z = 2.0f*3.1415f*a;
	v.x = cosf(v.z)*sq;
	v.y = sinf(v.z)*sq;
	v.z = b;
	return v;
}

glm::vec3 Utils::uniformSphere()
{
	glm::vec3 v;
	float theta = 2.0f * 3.1415f * random();
	float phi = acosf(1.0f - 2.0f * random());
	v.z = sinf(phi);
	v.x = v.z*cosf(theta);
	v.y = v.z*sinf(theta);
	v.z = cos(phi);
	return v;
}

glm::vec3 Utils::uniformCircle()
{
	glm::vec3 v;
	float a = 2.0f * 3.1415f*random();
	v.x = cosf(a);
	v.y = sinf(a);
	v.z = 0.0f;
	return v;
}

float Utils::random(float lo, float hi)
{
	if (generator != nullptr)
	{
		std::pair<float, float> key(lo, hi);
		if (uniformDistributions.find(key) == uniformDistributions.end())
		{
			uniformDistributions[key] = new std::uniform_real_distribution<float>(lo, hi);
			LOG_PRINT("Added uniform real distribution %f, %f", lo, hi);
		}
		return (*uniformDistributions[key])(*generator);
	}
	else
	{
		LOG_WARNING("Utils is not initialized!");
		return 0.0f;
	}
}