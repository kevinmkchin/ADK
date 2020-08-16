#pragma once

#include "Scene.h"

class ADKSaveLoad
{

public:
	// Save scene's entities to savePath + .json
	void SaveScene(const std::string& savePath, const Scene& scene);

	// Save entities to savePath + .json
	void SaveEntities(const std::string& savePath, const EntityList el);

	// Load savePath + .json to scene.Entities
	void LoadToScene(const std::string& savePath, Scene& scene);

};

