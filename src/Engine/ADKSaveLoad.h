#pragma once

#include "json.hpp"

class Entity;
class EntityList;
class Scene;

class ADKSaveLoad
{

public:
	// Save scene's entities to savePath + .json
	void save_scene(const std::string& savePath, const Scene& scene);

	// Save entities to savePath + .json
	void save_entities(const std::string& savePath, const EntityList el);

	// Load savePath + .json to scene.Entities
	void load_to_scene(const std::string& savePath, Scene& scene, bool b_delete_all_entities = true);

	// Save Prefab
	void save_prefab(const std::string& group_id, const std::string& prefab_id, Entity* const prefab_entity);

	// Load Prefab Group
	void load_prefab_group(const std::string& group_id, EntityList* list_to_load_to);

private:

	// Turns entity into a nholmann's json
	nlohmann::json jsonify_entity(Entity* const entity);

	void load_entities_from_json(nlohmann::json& entities_json, EntityList* list_to_load_to);
};

