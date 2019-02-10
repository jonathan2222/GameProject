#include "GameState.h"

#include "../../Engine/States/StateManager.h"
#include "../../Engine/Entity/EntityManager.h"
#include "../../Engine/Rendering/Display.h"
#include "../../Engine/Rendering/Renderer.h"

#include "../../Engine/Components/FreeMove.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/InputHandler.h"

GameState::GameState()
{
	EntityManager& entityManager = this->getEntityManager();

	Entity* roomEntity = entityManager.addEntity();
	roomEntity->setModel(ModelLoader::loadModel("./Game/assets/Room/Room.fbx"));

	Entity* cameraEntity = entityManager.addTracedEntity("Camera");
	cameraEntity->getTransform()->setPosition({0.0, 0.0, 4.0});
	new FreeMove(cameraEntity);
	Camera* camera = new Camera(cameraEntity, "Camera", { 0.0f, 0.0f, 0.0f });
	camera->init();
	Display::get().getRenderer().setActiveCamera(camera);

	InputHandler ih(Display::get().getWindowPtr());
}

GameState::~GameState()
{
}

void GameState::start()
{
}

void GameState::end()
{
}

void GameState::update(const float dt)
{
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();
	for (Entity* entity : entities)
		entity->update(dt);
}

void GameState::updateLogic()
{
}

void GameState::render()
{
	EntityManager& entityManager = this->getEntityManager();
	std::vector<Entity*>& entities = entityManager.getAll();

	Display& display = Display::get();
	Renderer& renderer = display.getRenderer();
	for (Entity* entity : entities)
		renderer.push(entity);
	renderer.drawAll();
}
