#include "ADKReflection.h"

ADKClassDatabase* ADKClassDatabase::instance = nullptr;

ADKClassDatabase::ADKClassDatabase()
{
}

ADKClassDatabase* ADKClassDatabase::get_database()
{
	if (instance == nullptr)
	{
		instance = new ADKClassDatabase();
	}

	return instance;
}

void ADKClassDatabase::register_class_description(ADKClassDescription* adkclass_desc)
{
	if (contains_class(adkclass_desc->type.name) == false)
	{
		// Store the pointer in the map with a key of the class name
		class_database[adkclass_desc->type.name.text] = adkclass_desc;
	}
}

ADKClassDescription* ADKClassDatabase::get_class_description(ADKName name)
{
	std::map<std::string, ADKClassDescription*>::iterator it = class_database.find(name.text);
	if (it != class_database.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

Entity* ADKClassDatabase::create_entity_of_class(ADKName name)
{
	// Find description of class requested
	ADKClassDescription* class_requested = get_class_description(name);
	// Call default constructor of class requested and return the object
	return class_requested->constructor();
}

bool ADKClassDatabase::contains_class(ADKName name)
{
	return class_database.find(name.text) != class_database.end();
}
