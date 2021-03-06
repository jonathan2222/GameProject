#pragma once

#include <vector>
#include "../Rendering/GLAbstraction/RenderingResources.h"
#include "../Rendering/GLAbstraction/VertexArray.h"
#include "../Rendering/GLAbstraction/IndexBuffer.h"
#include "../Rendering/GLAbstraction/UniformBuffer.h"

class Model;
class Mesh
{
public:
    Mesh(std::vector<Vertex>* vertices, std::vector<unsigned int>* vertexIndices, unsigned short materialIndex, Model* parent);
    ~Mesh();

    void bindVertexBuffer();
	IndexBuffer& getIndexBuffer();

    // Inserts material data into a uniform buffer
    void bindMaterial(UniformBuffer* uniformBuffer);
    unsigned short getMaterialIndex();

private:
    void createBuffers();

private:
    Model* parentModel;
    VertexArray* vao;
	IndexBuffer* ib;

    std::vector<Vertex>* vertices;
    std::vector<unsigned int>* vertexIndices;
    // The index refers to the material storage in the mesh's model
    unsigned short materialIndex;
};
