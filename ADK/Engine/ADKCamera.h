#pragma once
#include <SFML/Graphics.hpp>

class Entity;

/*	This class is the view manager for scenes. It updates the scene view every tick.
	Not every scene needs an ADKCamera/ViewManager. For example, the Scene_Editor already
	manages the editor view without a camera. However, a custom scene for a game such as
	a top-down scene or platformer scene might use an ADKCamera to control the view.
	The camera can follow a given entity target, perform a camera shake, etc. */
class ADKCamera
{

public:
	ADKCamera();

	ADKCamera(sf::RenderWindow& window);

	void update_camera(float dt);

	/*	Shake the camera
		x and y intensity determines how much the camera shakes, how far away from its original position it can shake. 
		lerp amount determines how slowly we return to normal; a lerp amount of 0.5f would mean the next tick/shake will
		be half as intense as the last tick/shake; a lerp amount of 1.f would mean the next tick/shake will be at the same
		intensity as the last tick/shake. duration determines how long the shake lasts for; it is possible for the camera 
		to essentially stop shaking before duration runs out due to lerping the intensity. b_at_intensity determines whether 
		the camera shake offset will always be at the current intensity values; if it is false, then the camera shake offset
		will be in range [-intensity, intensity]; this does not affect lerping.
	*/
	void shake_camera(float in_x_intensity, float in_y_intensity, float in_lerp_amount, float in_duration, bool b_at_intensity);

	void set_position(float in_x, float in_y);

	// setters getters
	void set_window(sf::RenderWindow& window);

	void set_follow_x(bool b_follow_x) { b_follow_target_x = b_follow_x; }
	bool is_following_x() const {  return b_follow_target_x; }
	void set_follow_y(bool b_follow_y) { b_follow_target_y = b_follow_y; }
	bool is_following_y() const { return b_follow_target_y; }

	void set_follow_target(Entity* in_entity);
	Entity* get_follow_target() const { return entity_target; }

private:
	sf::RenderWindow* render_window_ptr;

	sf::View active_camera_view;
	sf::Vector2f unmodified_center;

	// Whether the camera should follow the entity target
	bool b_follow_target_x;
	bool b_follow_target_y;
	// Can be nullptr if camera doesn't follow an entity
	Entity* entity_target;
	// Offset when following target
	sf::Vector2f follow_offset;

	// Camera shake
	sf::Vector2f camera_shake_offset;
	float x_intensity_original;
	float y_intensity_original;
	float shake_lerp_amount;
	float shake_duration;
	bool b_shake_at_intensity;
	float x_intensity_curr;
	float y_intensity_curr;

	// tolerance zone

	// follow speed

	// b_lerp_to_target

};

