#include "PlatformerMovingDamageBox.h"
#include "Engine/ADKMath.h"

ADKOBJECT(PlatformerMovingDamageBox)
PlatformerMovingDamageBox::PlatformerMovingDamageBox()
	: direction(0.f)
	, distance_to_travel(50.f)
	, travel_time_in_seconds(0.4f)
	, pause_at_turn_in_seconds(1.f)
	, b_going_towards_end(false)
	, travel_timer(0.f)
{
	ADKOBJECT_BEGIN(PlatformerMovingDamageBox)
	ADKOBJECT_END()

	texture_path = "Game/blackhole2.png";
	set_frame_size(16, 16);
	set_animation_start_frame(0, 0);
	sprite_sheet.animations[0].anim_duration = sf::seconds(0.4f);
	sprite_sheet.animations[0].num_frames = 8;

	init_collider();
}

void PlatformerMovingDamageBox::init_collider()
{
	collider = BoxCollider(get_position().x, get_position().y, 2.f, 2.f, 12.f, 12.f);
}

void PlatformerMovingDamageBox::update(float deltaTime)
{
	Entity::update(deltaTime);

	sf::Vector2f vel_this_tick = (b_going_towards_end ? 1.f : -1.f) * (velocity_per_sec * deltaTime);

	travel_timer += deltaTime;
	if (travel_timer < travel_time_in_seconds)
	{
		move((b_going_towards_end ? 1.f : -1.f) * (velocity_per_sec * deltaTime));
	}
	else if (travel_timer >= travel_time_in_seconds + pause_at_turn_in_seconds)
	{
		travel_timer = 0.f;
		b_going_towards_end = !b_going_towards_end;
	}
}

void PlatformerMovingDamageBox::begin_play()
{
	Entity::begin_play();

	sf::Vector2f travel_distance(distance_to_travel, 0.f);
	travel_distance = ADKMath::rotate_vector(travel_distance, direction);
	velocity_per_sec = travel_distance / travel_time_in_seconds;
}
