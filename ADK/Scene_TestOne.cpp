#include "Scene_TestOne.h"
#include "ADKEditorMetaRegistry.h"
#include "TopDownPlayer.h"

void Scene_TestOne::BeginScene(sf::RenderWindow& window)
{
	player = new TopDownPlayer();
	player->LoadDefaultTexture();
	Entities.add(player);
	player->SetPosition(500, 500);
	player->GetCollider().top = player->GetPosition().x;
	player->GetCollider().left = player->GetPosition().y;
	player->GetCollider().width = player->SpriteSheet.FrameSize.x;
	player->GetCollider().height = player->SpriteSheet.FrameSize.y;

	Entity* created = new Entity();
	created->SetTexturePathAndLoad("ajax.png");
	created->GetCollider().top = created->GetPosition().x;
	created->GetCollider().left = created->GetPosition().y;
	created->GetCollider().width = created->SpriteSheet.FrameSize.x;
	created->GetCollider().height = created->SpriteSheet.FrameSize.y;
	Entities.add(created);
}

void Scene_TestOne::Update(float deltaTime)
{
	Scene::Update(deltaTime);

}
