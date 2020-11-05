#pragma once

#include "Scene.h"

class ADKSaveLoad
{

public:
	// Save scene's entities to savePath + .json
	void save_scene(const std::string& savePath, const Scene& scene);

	// Save entities to savePath + .json
	void save_entities(const std::string& savePath, const EntityList el);

	// Load savePath + .json to scene.Entities
	void load_to_scene(const std::string& savePath, Scene& scene);

};

