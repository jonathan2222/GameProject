#pragma once

#include <string>
#include <vector>

#include "Shader.h"

class UniformBuffer
{
public:
	/*
		Maximum amount of binding points is 20
	*/
	UniformBuffer();
	~UniformBuffer();

	void setShader(unsigned shaderID, const std::string& blockName, unsigned bindingPoint);

	void setData(const void* const data, size_t dataSize);

	/*
		Replace specific part of uniform data, if offset(in bytes) + data size is bigger than the size of the uniform buffer return false and dosen't change anything
	*/
	bool setSubData(const void* const data, size_t dataSize, unsigned offset);

private:
	unsigned id, bindingPoint, currentSize;
};
