#include "Scene_TestOne.h"
#include "ADKEditorMetaRegistry.h"
#include "TopDownPlayer.h"

void Scene_TestOne::BeginScene(sf::RenderWindow& window)
{
	InitializeSceneView(window);

	player = new TopDownPlayer();
	player->LoadDefaultTexture();
	player->SetPosition(500.f, 500.f);
	player->InitCollider();
	Entities.add(player);

	created = new Entity(0.f, 0.f);
	created->SetTexturePathAndLoad("ajax.png");
	created->InitCollider();
	Entities.add(created);
}

void Scene_TestOne::Update(float deltaTime)
{
	Entities.Update(deltaTime);

	player->Move(player->GetCollider().ResolveCollisionRectangle(created->GetCollider()));
}

void Scene_TestOne::Render(sf::RenderWindow& window)
{
	Entities.Render(window);
	Entities.Render(window, true);
}
