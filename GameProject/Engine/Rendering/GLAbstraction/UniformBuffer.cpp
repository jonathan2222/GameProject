#include "UniformBuffer.h"

#include "GL/glew.h"

#include "../../../Utils/Logger.h"

UniformBuffer::UniformBuffer()
{
	
	glGenBuffers(1, &this->id);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &this->id);
}

void UniformBuffer::setShader(unsigned shaderID, const std::string & blockName, unsigned bindingPoint)
{
	glBindBuffer(GL_UNIFORM_BUFFER, this->id);
	this->bindingPoint = bindingPoint;
	unsigned int index = glGetUniformBlockIndex(shaderID, blockName.c_str());
	glUniformBlockBinding(shaderID, index, bindingPoint);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::setData(const void* const data, size_t dataSize)
{
	this->currentSize = dataSize;

	glBindBuffer(GL_UNIFORM_BUFFER, this->id);

	glBufferData(GL_UNIFORM_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, this->bindingPoint, this->id);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool UniformBuffer::setSubData(const void* const data, size_t dataSize, unsigned offset)
{
	if (offset + dataSize <= this->currentSize) {
		glBindBuffer(GL_UNIFORM_BUFFER, this->id);

		glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	LOG_WARNING("Data doesn't fit in uniform buffer! Data size: %d Free Data from offset: %d", dataSize, this->currentSize - offset);
	return false;
}
