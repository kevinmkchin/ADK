#include <iostream>
#include <fstream>
#include "ADKSaveLoad.h"
#include "json.hpp"

#include "ADKReflection.h"

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
	//json reflection_test;

	//Entity* ent_to_reflect = el.at(0);
	//char* ent_address = (char*) ent_to_reflect;
	//reflection_test["var name"] = ent_to_reflect->type->fields[0].name.text;
	//reflection_test["offset"] = ent_to_reflect->type->fields[0].offset;
	//reflection_test["value"] = *(ent_address + ent_to_reflect->type->fields[0].offset);

	//float offset_check = reflection_test["offset"];
	//float value_check = reflection_test["value"];

	//final["reflection_test"] = reflection_test;

	//json reflection_test_two;
	//Entity* ent_to_reflect = (Entity*) el.at(0);
	//char* ent_address = (char*) ent_to_reflect;
	//ADKClassDescription desc = ent_to_reflect->class_description;
	
	json ents;
	for (int i = 0; i < el.size(); ++i)
	{
		Entity* e = el.at(i);
		json item;

		item[ENTITY_ID_LABEL] = e->class_description_ptr->type.name.text;
		// Transform + Depth
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

		// SAVE FIELDS
		for (ADKFieldDescription field : e->class_description_ptr->fields)
		{
			json fieldjson;
			char* ent_address = (char*)e;
			std::string fieldtype = field.type.name.text;
			if (fieldtype == typeid(int).name())
			{
				fieldjson["type"] = field.type.name.text;
				fieldjson["offset"] = field.offset;
				fieldjson["value"] = *((int*)(ent_address + field.offset));
			}
			else if (fieldtype == typeid(float).name())
			{
				fieldjson["type"] = field.type.name.text;
				fieldjson["offset"] = field.offset;
				fieldjson["value"] = *((float*)(ent_address + field.offset));
			}
			else if (fieldtype == typeid(bool).name())
			{
				fieldjson["type"] = field.type.name.text;
				fieldjson["offset"] = field.offset;
				fieldjson["value"] = *((bool*)(ent_address + field.offset));
			}
			else if (fieldtype == typeid(std::string).name())
			{

			}
			item[field.name.text] = fieldjson;
		}

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
	load >> loaded;
	
	// Clear scene entities before loading (must delete before this line)
	if (b_clear_entities)
	{
		scene.level_entities.clear();
	}

	ADKClassDatabase* adkcdb = ADKClassDatabase::get_database();
	// Load each entity
	json entities = loaded[ENTITIES];
	for (json::iterator it = entities.begin(); it != entities.end(); ++it)
	{
		// Create a new entity
		json e = *it;

		ADKName entity_name;
		entity_name.text = e[ENTITY_ID_LABEL];
		Entity* created = adkcdb->create_entity_of_class(entity_name);

		// LOAD FIELDS
		// offset in json not being used, just getting the fields that class description knows about
		// type in json not being used
		char* ent_address = (char*)created;
		for (ADKFieldDescription field : created->class_description_ptr->fields)
		{
			int offset = field.offset;
			std::string fieldtype = field.type.name.text;
			if (fieldtype == typeid(int).name())
			{
				int* int_address = (int*)(ent_address + offset);
				*(int_address) = e[field.name.text]["value"];
			}
			else if (fieldtype == typeid(float).name())
			{
				float* float_address = (float*)(ent_address + offset);
				*(float_address) = e[field.name.text]["value"];
			}
			else if (fieldtype == typeid(bool).name())
			{
				bool* bool_address = (bool*)(ent_address + offset);
				*(bool_address) = e[field.name.text]["value"];
			}
			else if (fieldtype == typeid(std::string).name())
			{

			}
		}

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