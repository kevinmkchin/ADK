#include <iostream>
#include <fstream>
#include "ADKSaveLoad.h"

#include "Scene.h"
#include "ADKReflection.h"

#define LEVELPATH "Assets/Levels/"

#define ENTITIES "entities"
#define ENTITY_ID_LABEL "eid"
#define TEXTURE_LABEL "tpath"
#define ACTIVE_LABEL "act"
#define VISIBLE_LABEL "vis"
#define POS_X_LABEL "posx"
#define POS_Y_LABEL "posy"
#define ORIGIN_X "ogx"
#define ORIGIN_Y "ogy"
#define B_ORIGIN_FOR_POS "ogfp"
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
#define PREFAB_GROUP "pfg"
#define PREFAB_ID "pfi"

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
		ents += jsonify_entity(e);
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

void ADKSaveLoad::load_to_scene(const std::string& savePath, Scene& scene, bool b_delete_all_entities)
{
	if (savePath.empty())
	{
		return;
	}

	// Clear scene entities before loading (must delete before this line)
	if (b_delete_all_entities)
	{
		scene.level_entities.remove_and_destroy_all();
	}

	bool has_levelpath = savePath.find(LEVELPATH) == 0;
	bool has_json = (savePath.length() > 5) && (savePath.substr(savePath.length() - 5) == ".json");
	std::string path_to_load = (has_levelpath ? "" : LEVELPATH) + savePath + (has_json ? "" : ".json");

	// Load json
	std::ifstream load(path_to_load);
	json loaded;
	load >> loaded;

	json entities = loaded[ENTITIES];
	load_entities_from_json(entities, &scene.level_entities);
}

void ADKSaveLoad::save_prefab(const std::string& group_id, const std::string& prefab_id, Entity* const prefab_entity)
{
	if (group_id.empty() || prefab_id.empty() || prefab_entity == nullptr)
	{
		return;
	}

	json prefabs_json;
	std::string path = "Saved/Prefabs/" + group_id + ".json";

	// Load prefabs group
	std::ifstream prefabs_load(path);
	if (prefabs_load.good())
	{
		prefabs_load >> prefabs_json;
	}

	// Change prefab within group
	json prefab = jsonify_entity(prefab_entity);
	prefabs_json[prefab_id] = prefab;

	// Save Prefabs Json
	std::ofstream prefabs_stream_engine;
	prefabs_stream_engine.open(path);
	prefabs_stream_engine << prefabs_json.dump(4);
	prefabs_stream_engine.close();
}

void ADKSaveLoad::load_prefab_group(const std::string& group_id, EntityList* list_to_load_to)
{
	std::string path = "Saved/Prefabs/" + group_id;

	// Load json
	std::ifstream load(path);
	json loaded;
	load >> loaded;

	//json prefabs = loaded["PREFABS"];
	load_entities_from_json(loaded, list_to_load_to);
}

json ADKSaveLoad::jsonify_entity(Entity* const entity)
{
	Entity* const e = entity;
	json item;

	// TODO Save load TAGS

	item[ENTITY_ID_LABEL] = e->class_description_ptr->type.name.text;
	// Transform + Depth
	item[TEXTURE_LABEL] = e->get_texture_path();
	item[ACTIVE_LABEL] = e->is_active();
	item[VISIBLE_LABEL] = e->is_visible();
	item[POS_X_LABEL] = e->get_position().x;
	item[POS_Y_LABEL] = e->get_position().y;
	item[ORIGIN_X] = e->get_origin().x;
	item[ORIGIN_Y] = e->get_origin().y;
	item[B_ORIGIN_FOR_POS] = e->is_using_origin_for_position();
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
	// Prefab id
	item[PREFAB_GROUP] = e->prefab_group;
	item[PREFAB_ID] = e->prefab_id;

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

	return item;
}

void ADKSaveLoad::load_entities_from_json(json& entities_json, EntityList* list_to_load_to)
{
	ADKClassDatabase* adkcdb = ADKClassDatabase::get_database();
	// Load each entity
	for (json::iterator it = entities_json.begin(); it != entities_json.end(); ++it)
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
			// TODO edit this so if json doesn't have a newly added class field, we can just not do anything and use default value from constructor
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

		// ORDER HERE IS IMPORTANT!!!*************

		// Scale and origin first
		created->set_scale(e[SCALE_LABEL]);
		created->use_origin_for_position(e[B_ORIGIN_FOR_POS]);
		created->set_origin(sf::Vector2f(e[ORIGIN_X], e[ORIGIN_Y]));
		// Load the rest of the entity data
		created->set_position(e[POS_X_LABEL], e[POS_Y_LABEL]);
		created->set_rotation(e[ROT_LABEL]);
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
		// Prefab id
		std::string temp_str = e[PREFAB_GROUP];
		strcpy_s(created->prefab_group, temp_str.c_str());
		temp_str = e[PREFAB_ID];
		strcpy_s(created->prefab_id, temp_str.c_str());

		// Add the created entity to scene
		list_to_load_to->add(created);
	}
}
