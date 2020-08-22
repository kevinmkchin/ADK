#include "Scene_TestOne.h"
#include "ADKEditorMetaRegistry.h"
#include "TopDownPlayer.h"
#include <iostream>

void Scene_TestOne::BeginScene(sf::RenderWindow& window)
{
	player = new TopDownPlayer();
	player->LoadDefaultTexture();
	Entities.add(player);
	player->SetPosition(500.f, 500.f);
	player->GetCollider().left = player->GetPosition().x;
	player->GetCollider().top = player->GetPosition().y;
	player->GetCollider().width = (float) player->SpriteSheet.FrameSize.x;
	player->GetCollider().height = (float) player->SpriteSheet.FrameSize.y;

	created = new Entity(0.f, 0.f);
	created->SetTexturePathAndLoad("ajax.png");
	created->GetCollider().left = created->GetPosition().x;
	created->GetCollider().top = created->GetPosition().y;
	created->GetCollider().width = (float) created->SpriteSheet.FrameSize.x;
	created->GetCollider().height = (float) created->SpriteSheet.FrameSize.y;
	Entities.add(created);

	ViewConfig.CenterX = 400.f;
	ViewConfig.CenterY = 400.f;
	ViewConfig.Zoom = 0.5f;
	InitializeSceneView(window);
}

void Scene_TestOne::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	player->Move(player->GetCollider().ResolveCollisionRectangle(created->GetCollider()));
}
