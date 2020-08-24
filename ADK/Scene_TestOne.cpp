#include "Scene_TestOne.h"
#include "ADKEditorMetaRegistry.h"
#include "TopDownPlayer.h"

void Scene_TestOne::BeginScene(sf::RenderWindow& window)
{
	ViewConfig.CenterX = 430.f;
	ViewConfig.CenterY = 430.f;
	ViewConfig.Zoom = 0.15f;
	InitializeSceneView(window);

	player = new TopDownPlayer();
	player->LoadDefaultTexture();
	player->SetPosition(500.f, 500.f);
	player->InitCollider();
	player->SetVisible(false);
	Entities.add(player);

	created = new Entity(0.f, 0.f);
	created->SetTexturePathAndLoad("ajax.png");
	created->GetCollider() = BoxCollider(created->GetPosition().x, created->GetPosition().y, (float)created->SpriteSheet.FrameSize.x, (float)created->SpriteSheet.FrameSize.y);
	created->SetVisible(false);
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
