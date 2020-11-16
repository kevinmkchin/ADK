#include <iostream>
#include <fstream>
#include "ADKSaveLoad.h"
#include "../ADKEditorMetaRegistry.h"
#include "json.hpp"

#define LEVELPATH "Assets\\Levels\\"

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
#define COLLEFT_LABEL "cle"
#define COLTOP_LABEL "cto"
#define COLOFFSETX_LABEL "cox"
#define COLOFFSETY_LABEL "coy"
#define COLWIDTH_LABEL "cwi"
#define COLHEIGHT_LABEL "che"

using json = nlohmann::json;

void ADKSaveLoad::save_scene(const std::string& savePath, const Scene& scene)
{
	if (savePath.empty())
	{
		return;
	}

	save_entities(savePath, scene.level_entities);
}

void ADKSaveLoad::save_entities(const std::string& savePath, EntityList el)
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
		item[ENTITY_ID_LABEL] = e->entity_id;
		item[TEXTURE_LABEL] = e->get_texture_path();
		item[ACTIVE_LABEL] = e->is_active();
		item[VISIBLE_LABEL] = e->is_visible();
		item[POS_X_LABEL] = e->get_position().x;
		item[POS_Y_LABEL] = e->get_position().y;
		item[ROT_LABEL] = e->get_rotation();
		item[SCALE_LABEL] = e->get_scale();
		item[DEPTH_LABEL] = e->get_depth();
		// Sprite Sheet
		item[FRAME_X_LABEL] = e->sprite_sheet.frame_size.x;
		item[FRAME_Y_LABEL] = e->sprite_sheet.frame_size.y;
		item[REPEAT_LABEL] = e->sprite_sheet.b_repeat;
		item[ANIM_INDEX_LABEL] = e->sprite_sheet.selected_animation;
		// Animations
		json anims;
		for (auto a : e->sprite_sheet.animations)
		{
			json anim;
			anim[NUMFRAMES_LABEL] = a.num_frames;
			anim[STARTFRAME_LABEL] = a.start_frame;
			anim[ANIMDUR_LABEL] = a.anim_duration.asSeconds();

			anims += anim;
		}
		item[ANIMATIONS] = anims;
		// Collisions
		item[COLLEFT_LABEL] = e->get_collider().left;
		item[COLTOP_LABEL] = e->get_collider().top;
		item[COLOFFSETX_LABEL] = e->get_collider().offset_x;
		item[COLOFFSETY_LABEL] = e->get_collider().offset_y;
		item[COLWIDTH_LABEL] = e->get_collider().width;
		item[COLHEIGHT_LABEL] = e->get_collider().height;

		ents += item;
	}
	final[ENTITIES] = ents;

	// Save to json
	std::string serialized = final.dump(4);
	std::ofstream save;	

	bool has_levelpath = savePath.find(LEVELPATH) == 0;
	bool has_json = (savePath.length() > 5) && (savePath.substr(savePath.length() - 5) == ".json");
	std::string path_to_save = (has_levelpath ? "" : LEVELPATH) + savePath + (has_json ? "" : ".json");

	save.open(path_to_save);
	save << serialized;
	save.close();
}

void ADKSaveLoad::load_to_scene(const std::string& savePath, Scene& scene, bool b_clear_entities)
{
	if (savePath.empty())
	{
		return;
	}

	bool has_levelpath = savePath.find(LEVELPATH) == 0;
	bool has_json = (savePath.length() > 5) && (savePath.substr(savePath.length() - 5) == ".json");
	std::string path_to_load = (has_levelpath ? "" : LEVELPATH) + savePath + (has_json ? "" : ".json");

	// Load json
	std::ifstream load(path_to_load);
	json loaded;
	load >> loaded; // big fucking props to nlohmann/json project
	
	// Clear scene entities before loading (must delete before this line)
	if (b_clear_entities)
	{
		scene.level_entities.clear();
	}

	// Load each entity
	json entities = loaded[ENTITIES];
	for (json::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		// Create a new entity
		json e = *it;
		Entity* created = ADKEditorMetaRegistry::CreateNewEntity(e[ENTITY_ID_LABEL]);
		
		// Load SpriteSheet data first, so that setting texture path and loading will set SpriteSheet data correctly
		created->sprite_sheet.frame_size.x = e[FRAME_X_LABEL];
		created->sprite_sheet.frame_size.y = e[FRAME_Y_LABEL];
		created->sprite_sheet.b_repeat = e[REPEAT_LABEL];
		created->sprite_sheet.selected_animation = e[ANIM_INDEX_LABEL];
		// Clear animations because by default there might be some animations loaded
		created->sprite_sheet.animations.clear();
		json animations = e[ANIMATIONS];
		// Load each animation
		for (size_t i = 0; i < animations.size(); ++i)
		{
			FAnimation anim;
			anim.num_frames = animations[i][NUMFRAMES_LABEL];
			anim.start_frame = animations[i][STARTFRAME_LABEL];
			anim.anim_duration = sf::seconds(animations[i][ANIMDUR_LABEL]);

			created->sprite_sheet.animations.push_back(anim);
		}
		// Load the rest of the entity data
		created->entity_id = e[ENTITY_ID_LABEL];
		created->set_position(e[POS_X_LABEL], e[POS_Y_LABEL]);
		created->set_rotation(e[ROT_LABEL]);
		created->set_scale(e[SCALE_LABEL]);
		created->set_depth(e[DEPTH_LABEL]);
		created->set_active(e[ACTIVE_LABEL]);
		created->set_visible(e[VISIBLE_LABEL]);
		created->set_texture_path_and_load(e[TEXTURE_LABEL]);
		// Collisions
		created->get_collider().left = e[COLLEFT_LABEL];
		created->get_collider().top = e[COLTOP_LABEL];
		created->get_collider().offset_x = e[COLOFFSETX_LABEL];
		created->get_collider().offset_y = e[COLOFFSETY_LABEL];
		created->get_collider().width = e[COLWIDTH_LABEL];
		created->get_collider().height = e[COLHEIGHT_LABEL];

		// Add the created entity to scene
		scene.level_entities.add(created);
	}
}