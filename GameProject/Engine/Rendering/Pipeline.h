#pragma once

#include "GLAbstraction/RenderingResources.h"
#include "GLAbstraction/Framebuffer.h"
#include "GLAbstraction/UniformBuffer.h"
#include "GLAbstraction/Shader.h"
#include "Engine/AssetManagement/ModelLoader.h"
#include "Engine/Components/Camera.h"

class Entity;

class Pipeline
{
public:
	Pipeline();
	~Pipeline();

	void aoPass(const std::vector<Entity*>& renderingList, Texture* depth);

	/*
		PrePassDepth will stop any draw calls from writing to the depth buffer. Everything drawn in this pass will be used for depth testing
	*/
	void prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen = false);

	/*
		Draw directly to the screen
	*/
	void drawToScreen(const std::vector<Entity*>& renderingList);

	/*
		Draw to framebuffer color texture, nothing will be visible on screen unless you draw the texture to a quad
	*/
	Texture* drawToTexture(const std::vector<Entity*>& renderingList);
	/*
		Use texture to draw to quad which cover the whole screen
	*/
	void drawTextureToQuad(Texture* tex);

	/*
		Generates depth texture for shadows, input entities who should give away shadows
	*/
	Texture* calcDirLightDepth(const std::vector<Entity*>& renderingList, const glm::vec3& lightDir);

	void setActiveCamera(Camera* camera);

	Framebuffer* getFbo();

private:
	Camera * camera;
	unsigned width, height;
	UniformBuffer* uniformBuffer;
	Framebuffer fbo;

	void draw(const std::vector<Entity*>& renderingList);
	void draw(const std::vector<Entity*>& renderingList, Shader* shader, bool useMaterial = true);
	void drawModel(Model * model, Shader* shader);
	void drawModelWithoutMaterial(Model * model);

	void updateFramebufferDimension(WindowResizeEvent* event);

	void prePassDepthOn();
	void prePassDepthOff();

	Shader* ZprePassShader;
	Shader* testShader;
	Shader* quadShader;
	Shader* aoShader;

	Texture* randTexture;
	int kernelSize;
	std::vector<float> kernel;

	Model* quad;

};

