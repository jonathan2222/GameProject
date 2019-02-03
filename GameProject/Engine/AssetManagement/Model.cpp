#include "Model.h"

Model::Model()
{
    VAO = 0;

    glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
}

Model::~Model()
{
    // Delete all meshes
    for (unsigned short i = 0; i < meshes.size(); i += 1) {
        delete meshes.at(i);
    }
}

void Model::addMesh(Mesh* mesh)
{
    meshes.push_back(mesh);
}

void Model::addMaterial(Material material)
{
    materials.push_back(material);
}

size_t Model::meshCount()
{
    return meshes.size();
}

size_t Model::materialCount()
{
    return materials.size();
}

Mesh* Model::getMesh(size_t index)
{
    return meshes.at(index);
}
