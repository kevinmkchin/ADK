#include "Scene_TestOne.h"
#include "ADKEditorMetaRegistry.h"
#include "TopDownPlayer.h"

void Scene_TestOne::BeginScene(sf::RenderWindow& window)
{
	player = new TopDownPlayer();
	player->LoadDefaultTexture();
	Entities.add(player);
	player->SetPosition(500, 500);

	Entity* created = new Entity();
	created->SetTexturePathAndLoad("ajax.png");
	Entities.add(created);
}

void Scene_TestOne::Update(float deltaTime)
{
	Scene::Update(deltaTime);

}
