#include <iostream>
#include <fstream>
#include "ADKSaveLoad.h"
#include "../ADKEditorMetaRegistry.h"
#include "json.hpp"

#define LEVELPATH "Assets/Levels/"

#define ENTITIES "entities"
#define ENTITY_ID_LABEL "eid"
#define TEXTURE_LABEL "tpath"
#define ACTIVE_LABEL "act"
#define VISIBLE_LABEL "vis"
#define POS_X_LABEL "posx"
#define POS_Y_LABEL "posy"
#define ROT_LABEL "rot"
#define SCALE_LABEL "scl"
#define DEPTH_LABEL "dep"
//#define SPRITE_COLOR_RGB
#define FRAME_X_LABEL "frx"
#define FRAME_Y_LABEL "fry"
#define REPEAT_LABEL "rpt"
#define ANIM_INDEX_LABEL "aid"
#define ANIMATIONS "anims"
#define NUMFRAMES_LABEL "nfr"
#define STARTFRAME_LABEL "sfr"
#define ANIMDUR_LABEL "adr"

using json = nlohmann::json;

void ADKSaveLoad::SaveScene(const std::string& savePath, const Scene& scene)
{
	if (savePath.empty())
	{
		return;
	}

	SaveEntities(savePath, scene.Entities);
}

void ADKSaveLoad::SaveEntities(const std::string& savePath, EntityList el)
{
	if (savePath.empty())
	{
		return;
	}

	json final;
	json ents;
	for (int i = 0; i < el.size(); ++i)
	{
		Entity* e = el.at(i);
		json item;
		// Transform + Depth
		item[ENTITY_ID_LABEL] = e->EntityId;
		item[TEXTURE_LABEL] = e->GetTexturePath();
		item[ACTIVE_LABEL] = e->IsActive();
		item[VISIBLE_LABEL] = e->IsVisible();
		item[POS_X_LABEL] = e->GetPosition().x;
		item[POS_Y_LABEL] = e->GetPosition().y;
		item[ROT_LABEL] = e->GetRotation();
		item[SCALE_LABEL] = e->GetScale();
		item[DEPTH_LABEL] = e->GetDepth();
		// Sprite Sheet
		item[FRAME_X_LABEL] = e->SpriteSheet.FrameSize.x;
		item[FRAME_Y_LABEL] = e->SpriteSheet.FrameSize.y;
		item[REPEAT_LABEL] = e->SpriteSheet.bRepeat;
		item[ANIM_INDEX_LABEL] = e->SpriteSheet.SelectedAnimation;
		// Animations
		json anims;
		for (auto a : e->SpriteSheet.Animations)
		{
			json anim;
			anim[NUMFRAMES_LABEL] = a.NumFrames;
			anim[STARTFRAME_LABEL] = a.StartFrame;
			anim[ANIMDUR_LABEL] = a.AnimDuration.asSeconds();

			anims += anim;
		}
		item[ANIMATIONS] = anims;

		ents += item;
	}
	final[ENTITIES] = ents;

	// Save to json
	std::string serialized = final.dump();
	std::ofstream save;
	save.open(LEVELPATH + savePath + ".json");
	save << serialized;
	save.close();
}

void ADKSaveLoad::LoadToScene(const std::string& savePath, Scene& scene)
{
	if (savePath.empty())
	{
		return;
	}

	// Load json
	std::ifstream load(LEVELPATH + savePath + ".json");
	json loaded;
	load >> loaded; // big fucking props to nlohmann/json project
	
	// Clear scene entities before loading
	scene.Entities.clear();

	// Load each entity
	json entities = loaded[ENTITIES];
	for (json::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		// Create a new entity
		json e = *it;
		Entity* created = ADKEditorMetaRegistry::CreateNewEntity(e[ENTITY_ID_LABEL]);
		
		// Load SpriteSheet data first, so that setting texture path and loading will set SpriteSheet data correctly
		created->SpriteSheet.FrameSize.x = e[FRAME_X_LABEL];
		created->SpriteSheet.FrameSize.y = e[FRAME_Y_LABEL];
		created->SpriteSheet.bRepeat = e[REPEAT_LABEL];
		created->SpriteSheet.SelectedAnimation = e[ANIM_INDEX_LABEL];
		// Clear animations because by default there might be some animations loaded
		created->SpriteSheet.Animations.clear();
		json animations = e[ANIMATIONS];
		// Load each animation
		for (size_t i = 0; i < animations.size(); ++i)
		{
			FAnimation anim;
			anim.NumFrames = animations[i][NUMFRAMES_LABEL];
			anim.StartFrame = animations[i][STARTFRAME_LABEL];
			anim.AnimDuration = sf::seconds(animations[i][ANIMDUR_LABEL]);

			created->SpriteSheet.Animations.push_back(anim);
		}
		// Load the rest of the entity data
		created->EntityId = e[ENTITY_ID_LABEL];
		created->SetPosition(e[POS_X_LABEL], e[POS_Y_LABEL]);
		created->SetRotation(e[ROT_LABEL]);
		created->SetScale(e[SCALE_LABEL]);
		created->SetDepth(e[DEPTH_LABEL]);
		created->SetActive(e[ACTIVE_LABEL]);
		created->SetVisible(e[VISIBLE_LABEL]);
		created->SetTexturePathAndLoad(e[TEXTURE_LABEL]);

		// Add the created entity to scene
		scene.Entities.add(created);
	}
}