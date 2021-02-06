#include "PlatformerSpawn.h"
#include "PlatformerPlayer.h"

ADKOBJECT(PlatformerSpawn)
PlatformerSpawn::PlatformerSpawn()
	: spawn_id(0)
	, b_activated(false)
{
	ADKOBJECT_BEGIN(PlatformerSpawn)
	ADKOBJECT_FIELD(spawn_id)
	ADKOBJECT_END()

	//texture_path = "Misc/platformer-old/sheet2.png";
	//set_frame_size(8, 8);
	//set_animation_start_frame(0, 5);
	//sprite_sheet.animations[0].anim_duration = sf::seconds(0.f);
	//sprite_sheet.animations[0].num_frames = 1;
}

void PlatformerSpawn::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 10.f, 16.f);
}

void PlatformerSpawn::collided(Entity* collided_entity)
{
	if (PlatformerPlayer* player = dynamic_cast<PlatformerPlayer*>(collided_entity))
	{
		b_activated = true;
	}
}