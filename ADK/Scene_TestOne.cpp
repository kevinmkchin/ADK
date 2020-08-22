#include "Scene_TestOne.h"
#include "ADKEditorMetaRegistry.h"
#include "TopDownPlayer.h"
#include <iostream>

void Scene_TestOne::BeginScene(sf::RenderWindow& window)
{
	player = new TopDownPlayer();
	player->LoadDefaultTexture();
	Entities.add(player);
	player->SetPosition(500, 500);
	player->GetCollider().left = player->GetPosition().x;
	player->GetCollider().top = player->GetPosition().y;
	player->GetCollider().width = player->SpriteSheet.FrameSize.x;
	player->GetCollider().height = player->SpriteSheet.FrameSize.y;

	created = new Entity(0, 0);
	created->SetTexturePathAndLoad("ajax.png");
	created->GetCollider().left = created->GetPosition().x;
	created->GetCollider().top = created->GetPosition().y;
	created->GetCollider().width = created->SpriteSheet.FrameSize.x;
	created->GetCollider().height = created->SpriteSheet.FrameSize.y;
	Entities.add(created);

	ViewConfig.CenterX = 400.f;
	ViewConfig.CenterY = 400.f;
	ViewConfig.Zoom = 0.2f;
	InitializeSceneView(window);

}

void Scene_TestOne::Update(float deltaTime)
{
	Scene::Update(deltaTime);


	//// Collision resolution
	//if (player->GetCollider().Intersects(created->GetCollider()))
	//{
	//	std::cout << "collide" << std::endl;
	//}

	//std::cout << "x" << player->GetCollider().ResolveCollisionRectangle(created->GetCollider()).x << std::endl;
	//std::cout << "y" << player->GetCollider().ResolveCollisionRectangle(created->GetCollider()).y << std::endl;
	player->Move(player->GetCollider().ResolveCollisionRectangle(created->GetCollider()));

}
