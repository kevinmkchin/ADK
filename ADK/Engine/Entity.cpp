#include "Entity.h"
#include "EntityList.h"
#include "ADKAssets.h"


//////////////////////////////////////////////////////////////
// --- BASE ENTITY CONSTRUCTORS AND DESTRUCTOR ---

ADKOBJECT(Entity)
Entity::Entity()
	: b_active(true)
	, b_visible(true)
	, b_collidable(false)
	, origin_private(sf::Vector2f(0.f,0.f))
	, b_use_origin_for_position_private(true)
	, b_loaded_texture(false)
{
	ADKOBJECT_BEGIN(Entity)
	ADKOBJECT_END()

	set_position(0.f, 0.f);
	set_rotation(0.f);
	set_scale(1.f);
	set_depth(0);
}

Entity::Entity(float x, float y)
	: b_active(true)
	, b_visible(true)
	, b_collidable(false)
	, origin_private(sf::Vector2f(0.f, 0.f))
	, b_use_origin_for_position_private(true)
	, b_loaded_texture(false)
{
	ADKOBJECT_BEGIN(Entity)
	ADKOBJECT_END()

	set_position(x, y);
	set_rotation(0.f);
	set_scale(1.f);
	set_depth(0);
}

Entity::Entity(float x, float y, float inRot, float inScale)
	: b_active(true)
	, b_visible(true)
	, b_collidable(false)
	, origin_private(sf::Vector2f(0.f, 0.f))
	, b_use_origin_for_position_private(true)
	, b_loaded_texture(false)
{
	ADKOBJECT_BEGIN(Entity)
	ADKOBJECT_END()

	set_position(x, y);
	set_rotation(inRot);
	set_scale(inScale);
	set_depth(0);
}

Entity::~Entity()
{
	if (b_loaded_texture)
	{
		// Decrement texture reference count
		ADKAssets::unload(texture_path);
	}
}

//////////////////////////////////////////////////////////////

void Entity::update(float deltaTime)
{
	// Call begin play on first ever update call
	if (b_begun_play == false)
	{
		begin_play();
	}

	// Do animation
	update_animations(deltaTime);
}

void Entity::render(sf::RenderTarget& target)
{
	if (b_visible && sprite_sheet.sprite.getTexture() != nullptr)
	{
		target.draw(sprite_sheet.sprite);
	}
}

void Entity::render_debug(sf::RenderTarget& target, sf::Color debug_color)
{
	// Debug collider
	sf::RectangleShape col;
	col.setPosition(collider.left, collider.top);
	col.setSize(sf::Vector2f(collider.width, collider.height));
	col.setFillColor(debug_color);
	target.draw(col);
}

void Entity::move(float x, float y)
{
	sprite_sheet.sprite.move(x, y);
	collider.move(x, y);
}

void Entity::move(sf::Vector2f delta)
{
	sprite_sheet.sprite.move(delta);
	collider.move(delta);
}

sf::Vector2f Entity::get_position() const
{
	sf::Vector2f out_pos = sprite_sheet.sprite.getPosition();
	if (b_use_origin_for_position_private == false)
	{
		out_pos -= origin_private * get_scale();
	}

	return out_pos;
}
void Entity::set_position(float x, float y)
{
	set_position(sf::Vector2(x, y));
}
void Entity::set_position(sf::Vector2f newPos)
{
	if (b_use_origin_for_position_private)
	{
		sprite_sheet.sprite.setPosition(newPos);
		collider.set_pos(newPos);
	}
	else
	{
		sprite_sheet.sprite.setPosition(newPos + (origin_private * get_scale()));
		collider.set_pos(newPos);
	}
}
float Entity::get_rotation() const
{
	return sprite_sheet.sprite.getRotation();
}
void Entity::set_rotation(float newRot, bool bAffectCollider) 
{
	sprite_sheet.sprite.setRotation(newRot);

	if (bAffectCollider)
	{
		sf::FloatRect bounds = sprite_sheet.sprite.getGlobalBounds();
		collider.set_pos(bounds.left, bounds.top);
		collider.width = bounds.width;
		collider.height = bounds.height;
	}
}
float Entity::get_scale() const
{
	return sprite_sheet.sprite.getScale().x;
}
void Entity::set_scale(float newScale, bool bAffectCollider)
{
	float old_scale_x = sprite_sheet.sprite.getScale().x;
	float old_scale_y = sprite_sheet.sprite.getScale().y;

	sprite_sheet.sprite.setScale(newScale, newScale);

	if (bAffectCollider)
	{
		collider.set_offsets((collider.offset_x / old_scale_x) * newScale, (collider.offset_y / old_scale_y) * newScale);
		collider.width = (collider.width / old_scale_y) * newScale;
		collider.height = (collider.height / old_scale_y) * newScale;
	}
}
void Entity::set_depth(int newDepth)
{
	depth = newDepth;

	// MARK ENTITY LIST DEPTH CHANGED for each EntityList that this entity belongs to
	for (EntityList* el : entitylists)
	{
		el->mark_depth_changed();
	}
}

void Entity::load_default_texture()
{
	set_texture_path_and_load(texture_path);
}

void Entity::set_texture_path_and_load(const std::string& path, bool forceNoUnload)
{
	// If we already have a texture, then unload that first
	if (sprite_sheet.sprite.getTexture() != nullptr && forceNoUnload == false)
	{
		// Early return if our current texture is the one we are trying to load
		if (texture_path == path)
		{
			return;
		}

		ADKAssets::unload(texture_path);
	}
	texture_path = path;
	sf::Texture& LoadedTexture = ADKAssets::get(texture_path);
	b_loaded_texture = true;
	sprite_sheet.sprite.setTexture(LoadedTexture, true);

	// If first time setting sprite sheet, set it to entire texture size by default
	if (sprite_sheet.frame_size.x == 0 && sprite_sheet.frame_size.y == 0)
	{
		match_framesize_to_texture();
	}
	else
	{
		sprite_sheet.sprite.setTextureRect(sf::IntRect((int)get_position().x, (int)get_position().y, sprite_sheet.frame_size.x, sprite_sheet.frame_size.y));
	}
}

void Entity::match_framesize_to_texture()
{
	set_frame_size(sprite_sheet.sprite.getTexture()->getSize().x, sprite_sheet.sprite.getTexture()->getSize().y);
	sprite_sheet.sprite.setTextureRect(sf::IntRect((int) get_position().x, (int)get_position().y, sprite_sheet.frame_size.x, sprite_sheet.frame_size.y));
}

void Entity::set_animation_start_frame(int anim_index, int start_frame)
{
	if ((std::size_t) anim_index >= sprite_sheet.animations.size())
	{
		printf("Attempted to set start frame of animation %d but there are only %d animations.\n", anim_index, sprite_sheet.animations.size());
		return;
	}

	sprite_sheet.animations[anim_index].start_frame = start_frame;
}

void Entity::set_origin(float x, float y)
{
	origin_private = sf::Vector2f(x, y);
	get_sprite().setOrigin(origin_private);
}

void Entity::set_origin(sf::Vector2f in_origin)
{
	origin_private = in_origin;
	get_sprite().setOrigin(origin_private);
}

void Entity::use_origin_for_position(bool b_use)
{
	b_use_origin_for_position_private = b_use;
}

bool Entity::add_entity_list(EntityList* new_list)
{
	// Check new_list isn't already added
	for (EntityList* el : entitylists)
	{
		if (el == new_list)
		{
			return false;
		}
	}
	// Keep track of new EntityList
	entitylists.push_back(new_list);
	return true;
}

bool Entity::remove_entity_list(EntityList* list_to_remove)
{
	std::vector<EntityList*>::iterator to_remove = std::find(entitylists.begin(), entitylists.end(), list_to_remove);
	int removal_index = std::distance(entitylists.begin(), to_remove);
	if (removal_index == entitylists.size())
	{
		return false;
	}
	// Erase entitylist
	entitylists.erase(to_remove);
	return true;
}

void Entity::assign_tag(Tags tag[], int size = 1)
{
	for (int i, j = 0; i < 8; ++i)
	{
		if (tags[i] == TAG_DEFAULT)
		{
			tags[i] = tag[j];
			++j;
			if (j >= size)
			{
				break;
			}
		}
	}
}

void Entity::remove_tag(Tags tag[], int size = 1)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (tag[i] == tags[j])
			{
				tags[j] = TAG_DEFAULT;
			}
		}
	}
}

bool Entity::has_tag(Tags tag[], int size)
{
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (tag[i] == tags[j])
			{
				return true;
			}
		}
	}
	return false;
}

void Entity::get_tags(Tags tag[], int size)
{
	char* to = (char*) tag;
	const char* from = (char*) tags;
	strncpy(to, from, 8);
}

void Entity::set_frame_size(int x, int y)
{
	sprite_sheet.frame_size.x = x;
	sprite_sheet.frame_size.y = y;
}

void Entity::begin_play()
{
	b_begun_play = true;
}

sf::Sprite& Entity::get_sprite()
{
	return sprite_sheet.sprite;
}

void Entity::init_collider()
{

}

void Entity::collided(Entity* collided_entity)
{

}

void Entity::copy(Entity& target, const Entity& source)
{
	// Copy base entity fields
	target.sprite_sheet = source.sprite_sheet;
	target.set_frame_size(source.sprite_sheet.frame_size.x, source.sprite_sheet.frame_size.y);
	target.set_texture_path_and_load(source.get_texture_path(), true);
	target.set_active(source.is_active());
	target.set_visible(source.is_visible());
	target.set_rotation(source.get_rotation());
	target.set_scale(source.get_scale());
	target.set_depth(source.get_depth());
	target.get_collider().offset_x = source.get_collider_copy().offset_x;
	target.get_collider().offset_y = source.get_collider_copy().offset_y;
	target.set_position(source.get_position());
	target.get_collider().width = source.get_collider_copy().width;
	target.get_collider().height = source.get_collider_copy().height;
	target.set_collidable(source.is_collidable());
	strcpy_s(target.prefab_group, source.prefab_group);
	strcpy_s(target.prefab_id, source.prefab_id);

	// Copy class specific fields ONLY if target is exact same type as source
	if (target.class_description_ptr == source.class_description_ptr)
	{
		char* source_address = (char*)&source;
		char* target_address = (char*)&target;
		std::vector<ADKFieldDescription> fields = source.class_description_ptr->fields;
		for (ADKFieldDescription field : fields)
		{
			std::string fieldtype = field.type.name.text;
			if (fieldtype == typeid(int).name())
			{
				int* t_int_address = (int*)(target_address + field.offset);
				int* s_int_address = (int*)(source_address + field.offset);
				*(t_int_address) = *(s_int_address);
			}
			else if (fieldtype == typeid(float).name())
			{
				float* t_int_address = (float*)(target_address + field.offset);
				float* s_int_address = (float*)(source_address + field.offset);
				*(t_int_address) = *(s_int_address);
			}
			else if (fieldtype == typeid(bool).name())
			{
				bool* t_int_address = (bool*)(target_address + field.offset);
				bool* s_int_address = (bool*)(source_address + field.offset);
				*(t_int_address) = *(s_int_address);
			}
			else if (fieldtype == typeid(std::string).name())
			{

			}
		}
	}

	// TODO copy Tag data
}

void Entity::update_animations(float deltaTime)
{
#pragma region AnimationUpdate

	// Get num frames wide and tall
	sf::Vector2i textureBounds(sprite_sheet.sprite.getTexture()->getSize());
	int numFramesWide = textureBounds.x / ((sprite_sheet.frame_size.x > 0) ? sprite_sheet.frame_size.x : 1);
	int numFramesTall = textureBounds.y / ((sprite_sheet.frame_size.y > 0) ? sprite_sheet.frame_size.y : 1);

	FAnimation currAnim = sprite_sheet.animations[sprite_sheet.selected_animation];
	int framesInAnim = static_cast<int>(currAnim.num_frames) < numFramesTall * numFramesWide ? static_cast<int>(currAnim.num_frames) : numFramesTall * numFramesWide;
	sf::Time timePerFrame = currAnim.anim_duration / (float)framesInAnim;

	if (b_visible 
		&& sprite_sheet.sprite.getTexture() != nullptr 
		&& sprite_sheet.animations.empty() == false 
		&& b_anim_paused == false)
	{
		sprite_sheet.elapsed_time += sf::seconds(deltaTime);

		// Update current frame
		if (timePerFrame.asSeconds() > 0 && currAnim.anim_duration.asSeconds() > 0.f && currAnim.num_frames > 0)
		{
			while (sprite_sheet.elapsed_time > timePerFrame && (sprite_sheet.current_frame - currAnim.start_frame <= currAnim.num_frames || sprite_sheet.b_repeat))
			{
				if (sprite_sheet.b_repeat)
				{
					sprite_sheet.current_frame = currAnim.start_frame + ((sprite_sheet.current_frame - currAnim.start_frame + 1) % currAnim.num_frames);
				}
				else
				{
					// Go to next frame if we aren't on the last frame
					if (sprite_sheet.current_frame < currAnim.num_frames - 1)
					{
						++sprite_sheet.current_frame;
					}
				}

				// Check if we went out of bounds
				if (static_cast<int>(sprite_sheet.current_frame) >= numFramesWide * numFramesTall)
				{
					sprite_sheet.current_frame = currAnim.start_frame;
				}

				// Subtract from elapsed time since last frame
				sprite_sheet.elapsed_time -= timePerFrame;
			}
		}
		else if (currAnim.anim_duration.asSeconds() == 0.f)
		{
			// If anim duration == 0, then static sprite at start frame
			sprite_sheet.current_frame = currAnim.start_frame;
		}

		// by this point current frame will be accurate with frame we need to display
	}

	// Figure out texture rect from current frame
	int xLoc = (sprite_sheet.current_frame % (numFramesWide > 0 ? numFramesWide : 1)) * sprite_sheet.frame_size.x;
	int yLoc = (sprite_sheet.current_frame / (numFramesWide > 0 ? numFramesWide : 1)) * sprite_sheet.frame_size.y;
	sf::IntRect frameRect = sf::IntRect(xLoc, yLoc, sprite_sheet.frame_size.x, sprite_sheet.frame_size.y);
	// Set new frame to display
	sprite_sheet.sprite.setTextureRect(frameRect);
#pragma endregion
}