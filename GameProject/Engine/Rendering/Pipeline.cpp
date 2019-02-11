#include "Pipeline.h"

#include "Engine/AssetManagement/Mesh.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Events/EventBus.h"
#include "Utils/Utils.h"

Pipeline::Pipeline()
{
	EventBus::get().subscribe(this, &Pipeline::updateFramebufferDimension);

	this->quad = ModelLoader::loadModel("Game/assets/postProcessQuad.fbx");

	/*
		Loading in shaders, Quad and Z pre pass are necessary for drawing texture to window and depth pre pass
		Test shader will be swapped out with a option to choose between multiple shaders for rendering Entities
	*/
	this->quadShader = new Shader("./Engine/Rendering/Shaders/PostProcessVert.vert", "./Engine/Rendering/Shaders/PostProcessFrag.frag");
	this->testShader = new Shader("./Engine/Rendering/Shaders/EntityShader.vert", "./Engine/Rendering/Shaders/EntityShader.frag");
	this->ZprePassShader = new Shader("./Engine/Rendering/Shaders/ZPrepassVert.vert", "./Engine/Rendering/Shaders/ZPrepassFrag.frag");
	this->aoShader = new Shader("./Engine/Rendering/Shaders/AOFactor.vert", "./Engine/Rendering/Shaders/AOFactor.frag");
	Display& display = Display::get();
	int width = display.getWidth();
	int height = display.getHeight();
	this->fbo.attachTexture(width, height, AttachmentType::COLOR);
	this->fbo.attachTexture(width, height, AttachmentType::DEPTH);

	/*
		Set up main uniform buffer for material settings
	*/
	this->uniformBuffer = new UniformBuffer();
	this->uniformBuffer->setShader(this->testShader->getID(), "Material", 0);
	Material emptyMaterial;
	emptyMaterial.Ka = glm::vec3(0.1f);
	emptyMaterial.Ks = glm::vec3(1.0f);
	this->uniformBuffer->setData((void*)(&emptyMaterial), sizeof(emptyMaterial) - sizeof(emptyMaterial.textures));

	// Setup the kernel with random directions from a hemisphere.
	this->kernelSize = 64;
	for (int i = 0; i < kernelSize; i++)
	{
		glm::vec3 randDir = Utils::uniformHemisphere();
		//float scale = ((float)(i*i)) / ((float)(kernelSize*kernelSize));
		//randDir *= glm::mix(0.1f, 1.0f, scale);
		this->kernel.push_back(randDir.x);
		this->kernel.push_back(randDir.y);
		this->kernel.push_back(randDir.z);
	}

	// Setup the texture with random directions from a circle in the xy-plane.
	const int size = 4;
	std::vector<GLfloat> randDirections(3 * size * size);
	for (int i = 0; i < size*size; i++)
	{
		glm::vec3 v = Utils::uniformCircle();
		randDirections[i * 3 + 0] = v.x;
		randDirections[i * 3 + 1] = v.y;
		randDirections[i * 3 + 2] = v.z;
	}
	this->randTexture = new Texture(randDirections.data(), size, size, GL_RGB16F, GL_RGB, GL_FLOAT);
	this->randTexture->unbind();
}


Pipeline::~Pipeline()
{
	delete this->quadShader;
	delete this->ZprePassShader;
	delete this->testShader;
	delete this->aoShader;
	delete this->uniformBuffer;
	delete this->randTexture;
}

void Pipeline::aoPass(const std::vector<Entity*>& renderingList, Texture* depth)
{
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

	this->aoShader->bind();

	this->aoShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	this->aoShader->setUniformMatrix4fv("view", 1, false, &(this->camera->getView()[0][0]));
	this->aoShader->setUniformMatrix4fv("proj", 1, false, &(this->camera->getProj()[0][0]));
	this->aoShader->setTexture2D("depthTex", 0, *depth);
	this->aoShader->setTexture2D("randTex", 1, *this->randTexture);
	this->aoShader->setUniform3fv("sampleKernel", this->kernelSize, this->kernel.data());
	draw(renderingList, this->aoShader, false);

	this->aoShader->unbind();
}

void Pipeline::prePassDepth(const std::vector<Entity*>& renderingList, bool toScreen)
{
	if(!toScreen)
		this->fbo.bind();
	this->prePassDepthOn();
	this->ZprePassShader->bind();

	//Draw renderingList
	this->ZprePassShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	draw(renderingList);
	
	this->ZprePassShader->unbind();
	this->prePassDepthOff();
	if (!toScreen)
		this->fbo.unbind();
}

void Pipeline::prePassDepthOn()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glClear(GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LESS);
	glColorMask(0, 0, 0, 0);

}

void Pipeline::prePassDepthOff()
{
	glDepthFunc(GL_LEQUAL);
	glColorMask(1, 1, 1, 1);
	glDepthMask(GL_FALSE);
}

void Pipeline::drawToScreen(const std::vector<Entity*>& renderingList)
{
	glEnable(GL_DEPTH_TEST);

	//shader->bind();
	this->testShader->bind();

	this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));
	draw(renderingList, this->testShader);

	this->testShader->unbind();
	//shader->unbind();
}

/*
	Draws to texture and returns it using shader provided
*/
Texture * Pipeline::drawToTexture(const std::vector<Entity*>& renderingList)
{
	glEnable(GL_DEPTH_TEST);

	this->fbo.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->testShader->bind();

	this->testShader->setUniformMatrix4fv("vp", 1, false, &(this->camera->getVP()[0][0]));

	draw(renderingList, this->testShader);

	this->testShader->unbind();
	this->fbo.unbind();

	return this->fbo.getColorTexture(0);
}

void Pipeline::drawTextureToQuad(Texture * tex)
{

	glDisable(GL_DEPTH_TEST);

	this->quadShader->bind();
	this->quadShader->setTexture2D("tex", 0, tex->getID());

	Mesh* mesh = this->quad->getMesh(0);
	mesh->bindVertexBuffer();
	IndexBuffer& ib = mesh->getIndexBuffer();
	ib.bind();
	glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);

	this->quadShader->unbind();
}

void Pipeline::setActiveCamera(Camera * camera)
{
	this->camera = camera;
}

Framebuffer * Pipeline::getFbo()
{
	return &this->fbo;
}

void Pipeline::draw(const std::vector<Entity*>& renderingList)
{
	for (Entity* entity : renderingList)
	{
		Model* model = entity->getModel();
		Transform* transform = entity->getTransform();

		if (model != nullptr)
		{
			this->ZprePassShader->setUniformMatrix4fv("transform", 1, false, &(transform->getMatrix()[0][0]));
			this->drawModelWithoutMaterial(model);
		}
	}
}

void Pipeline::draw(const std::vector<Entity*>& renderingList, Shader* shader, bool useMaterial)
{
	for (Entity* entity : renderingList)
	{
		Model* model = entity->getModel();
		Transform* transform = entity->getTransform();
		
		if (model != nullptr)
		{
			shader->setUniformMatrix4fv("transform", 1, false, &(transform->getMatrix()[0][0]));
			if (useMaterial)
				drawModel(model, shader);
			else
				drawModelWithoutMaterial(model);
		}
	}
}

void Pipeline::drawModelWithoutMaterial(Model * model)
{
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);

		mesh->bindVertexBuffer();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}

void Pipeline::updateFramebufferDimension(WindowResizeEvent * event)
{
	this->fbo.updateDimensions(0, event->width, event->height);
}

void Pipeline::drawModel(Model * model, Shader* shader)
{
	for (size_t i = 0; i < model->meshCount(); i++)
	{
		Mesh* mesh = model->getMesh(i);
		mesh->bindMaterial(this->uniformBuffer);

		unsigned int materialIndex = mesh->getMaterialIndex();
		Material& material = model->getMaterial(materialIndex);

		this->uniformBuffer->setSubData((void*)(&material), sizeof(material) - sizeof(material.textures), 0);

		for (Texture* texture : material.textures) {
			shader->setTexture2D("tex", 0, texture->getID());
		}

		mesh->bindVertexBuffer();
		IndexBuffer& ib = mesh->getIndexBuffer();
		ib.bind();
		glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, 0);
	}
}
