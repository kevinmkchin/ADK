#pragma once
#include <string>
#include <map>
#include <vector>

#pragma region ADK REFLECTION MACROS

#define GET_NAME(ClassOrField) (#ClassOrField)

/**	This class type will be created at the start of the program. Do this for each reflected class type so that 
	all reflected type constructors are called at the start of the program. */
#define ADKOBJECT(classtype) \
	static classtype static_instance;

#define ADKOBJECT_BEGIN(classtype) \
	ADKName name_check { -1, GET_NAME(classtype) }; \
	if (ADKClassDatabase::get_database()->contains_class(name_check)) { \
		class_description_ptr = ADKClassDatabase::get_database()->get_class_description(name_check); } \
	else { \
		ADKClassDescription* super_class_description_ptr = class_description_ptr; \
		class_description_ptr = new ADKClassDescription; \
		class_description_ptr->type.name.text = GET_NAME(classtype); \
		class_description_ptr->type.size = sizeof(classtype); \
		class_description_ptr->constructor = &default_constructor<classtype>; \
		if (super_class_description_ptr != nullptr) { \
			class_description_ptr->fields = super_class_description_ptr->fields; \
		} \
		using T = classtype; \
		ADKFieldDescription field_to_add;

#define ADKOBJECT_FIELD(field) \
		field_to_add = ADKFieldDescription(); \
		field_to_add.name.text = GET_NAME(field); \
		field_to_add.type.name.text = typeid(field).name(); \
		field_to_add.type.size = sizeof(field); \
		field_to_add.offset = offsetof(T, field); \
		class_description_ptr->fields.push_back(field_to_add);

#define ADKOBJECT_END() \
		ADKClassDatabase::get_database()->register_class_description(class_description_ptr); \
	}

#pragma endregion

class Entity;

/** Some basic class and field reflection descriptions for ADK */

// Holds a name, maybe a hash instead for name?
struct ADKName
{
	// TODO maybe start using hash somehow?
	int hash;
	std::string text;
};

// Describes a type. Name of type (e.g. "Entity", "i" for int, "Ss" for std::string)
struct ADKType
{
	ADKName name;
	int size;
};

// Describes a member field. A field is a tuple of Name, Type, Offset
struct ADKFieldDescription
{
	ADKName name;	// This is the field name; if a field is "float rotation;" then name.text would be "rotation". This is used for human reading.
	ADKType type;	// This is the type; the type name is the name of type of member (e.g. "i" for int, "Ss" for std::string, "class Entity", etc).
	int offset;		// Location in memory offset from class ptr
};

// Describes a class with member fields. Contains a pointer to default constructor, type of class, and a vector member fields.
struct ADKClassDescription
{
	Entity*(*constructor)();
	ADKType type;
	std::vector<ADKFieldDescription> fields;
};

// Wrapper function for default constructor
template<typename T>
Entity * default_constructor() { return new T; }

// Base class for all ADK reflected objects 
class ADKObject
{
public:
	ADKObject()
	{
		class_description_ptr = nullptr;
	}

	ADKClassDescription* class_description_ptr;
};

/*	ADKClassDatabase is a singleton class */
class ADKClassDatabase
{
private:
	static ADKClassDatabase* instance;
	ADKClassDatabase();
	std::map<std::string, ADKClassDescription*> class_database; // TODO can this be unordred_map?

public:
	// Grab the static database instance
	static ADKClassDatabase* get_database();

	// Register class description
	void register_class_description(ADKClassDescription* adkclass_desc);

	// Get class description
	ADKClassDescription* get_class_description(ADKName name);

	// Create object
	Entity* create_entity_of_class(ADKName name);

	// Checks if the description of a class with given name is registered
	bool contains_class(ADKName name);

	// Get a copy of the database map
	std::map<std::string, ADKClassDescription*> get_all_class_descriptions() const { return class_database; }
};

/**	ADK REFLECTION GUIDE

	We can either reflect, simple reflect, or not reflect the ADKObject. If we reflect an ADKObject, then we store the 
	ADKObject's default constructor, type, and member fields in the class description and register that description.
	This allows us to find out use that class description to find out what information to display in the editor or save 
	and load from the editor to the game. The type holds a name and a size. The name is a string representation of the 
	class type. The name for class Entity would be "Entity" and the name for class PlatformerMovingPlatform would be 
	"PlatformerMovingPlatform". The size represents the byte size of the class (not being used right now but might come 
	in handy in the future). The class description also stores a vector of ADKFieldDescription. Each ADKFieldDescription
	describes a reflected field member of the class.

	Calling the macro ADKOBJECT_FIELD on a class member field will add a description of that field to the class description.
	A field descriptions is a tuple of name, type, and offset. The name is a human readable string representation of the class 
	member name. For example, a field of "int health" would have a description name of "health". The type is an ADKType which 
	consists of a name and size. The name holds an identifier for the field type (int, float, std::string, etc.), and the size 
	represents the memory size of the field (not being used right now but might come in handy in the future). The offset is the 
	memory location offset from the class ptr. Only fields with these descriptions will be exposed to the editor and subsequently 
	saved and loaded between the editor and the game. 

	A simple reflection is done using ADKOBJECT (outside of the constructor as always) and ADKOBJECT_SIMPLE. A macro call of
	ADKOBJECT_SIMPLE is essentially identical to calling ADKOBJECT_BEGIN and then immediately ADKOBJECT_END with no field
	reflections in between. A simple reflection registers a description of the class but does not provide it any field members.

	Not reflecting an ADKObject means not registering a description for that class. If an ADKObject isn't ADK reflected 
	(i.e. not marked with ADKOBJECT, ADKOBJECT_BEGIN, and ADKOBJECT_END) then their class_description_ptr will be
	pointing to the class description of the closest parent class who is ADK reflected. 
	
*/