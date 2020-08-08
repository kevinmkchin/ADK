#pragma once

#include "Engine/Entity.h"
#include "ExampleEntity.h"

#pragma region ignore1

#define DECLARE(id) id,
#define REGISTER(type, id) m[id] = &create_instance<type>;

struct ADKEditorMetaRegistry
{
public:
	const static inline std::vector<std::string> Identifiers = { 
#pragma endregion
	////////////////////////////////////////////////////////// TODO REGISTER ENTITY TYPES AND IDENTIFIERS ///////////////////////////////////////////////////////////////

	// REMEMBER TO TYPE " REGISTER_ENTITY_TYPES " AT THE START OF THE GAME BEFORE USING ANY OF THIS

	// DECLARE IDENTIFIERS HERE
	DECLARE("BaseEntity")
	DECLARE("ExampleEntity")

#pragma region ignore2
	};

	static std::map<std::string, Entity*(*)()> create_map() { std::map<std::string, Entity*(*)()> m;
#pragma endregion
	/* TODO Add every entity type and the identifier you want to use for them here, that you want to see in the ADK level editor and use in ADK level data, here.
	Format:
				REGISTER( YOUR_ENTITY_TYPE , " YOUR_IDENTIFIER ")
	*/
	
	// REGISTER ENTITY TYPES TO IDENTIFIERS HERE
	REGISTER(Entity, "BaseEntity")
	REGISTER(ExampleEntity, "ExampleEntity")

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region ignore3
		return m; 
	}

	static Entity* CreateNewEntity(std::string id)
	{
		if (EntityTypeRegistry.count(id) == 0)
		{
			throw std::runtime_error("ADKEditorMetaRegistry::CreateNewEntity: given identifier '" + id + "' isn't associated with any Entity class." );
			return nullptr;
		}

		return EntityTypeRegistry.at(id)();
	}

private:
	template<typename T> 
	static Entity * create_instance() { return new T; }

public:
	static const std::map<std::string, Entity*(*)()> EntityTypeRegistry;

};

#define REGISTER_ENTITY_TYPES const std::map<std::string, Entity*(*)()> ADKEditorMetaRegistry::EntityTypeRegistry = ADKEditorMetaRegistry::create_map();

#pragma endregion