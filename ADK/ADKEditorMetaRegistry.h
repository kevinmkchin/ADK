#pragma once

#include "Engine/Entity.h"
#include "ExampleEntity.h"


#pragma region ignore1
struct ADKEditorMetaRegistry
{
public:
	const static inline std::vector<std::string> Identifiers = { 
#pragma endregion

	////////////////////////////////////////////////////////// TODO REGISTER ENTITY TYPES AND IDENTIFIERS ///////////////////////////////////////////////////////////////

	// DECLARE IDENTIFIERS HERE

	"BaseEntity", 
	"ExampleEntity",


#pragma region ignore2
	};

	static std::map<std::string, Entity*(*)()> CreateMap() { std::map<std::string, Entity*(*)()> m;
#pragma endregion

	/* TODO Add every entity type and the identifier you want to use for them here, that you want to see in the ADK level editor and use in ADK level data, here.
	FORMAT:

	m["YOUR_IDENTIFIER"] = &CreateInstance< YOUR_ENTITY_TYPE >;

	*/
	
	m["BaseEntity"]				= &CreateInstance<Entity>;
	m["ExampleEntity"]			= &CreateInstance<ExampleEntity>;
	//m["  "]						= &CreateInstance<  >;
	//m["  "]						= &CreateInstance<  >;
	//m["  "]						= &CreateInstance<  >;
	//m["  "]						= &CreateInstance<  >;
	//m["  "]						= &CreateInstance<  >;
	//m["  "]						= &CreateInstance<  >;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	static Entity * CreateInstance() { return new T; }

public:
	static const std::map<std::string, Entity*(*)()> EntityTypeRegistry;

};

#define REGISTER_ENTITY_TYPES const std::map<std::string, Entity*(*)()> ADKEditorMetaRegistry::EntityTypeRegistry = ADKEditorMetaRegistry::CreateMap();

#pragma endregion