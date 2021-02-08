#pragma once

class Engine;
namespace sf
{
	class RenderWindow;
}
class ADKTimer;
class Scene;

/*	This class represents several static pointers to game objects that are commonly shared between classes.
	This includes the SFML RenderWindow, the currently active ADK scene, */
class ADKGameStatics
{

public:

	///////////////////////////////////////////////////
	// You may add fields for your own game objects you would like to share between classes. e.g. player_object

	// static PlatformerPlayer* player;
	// static Tags some_tag;


	// Pointers either need to be inline (like engine below) or initialized in the cpp.
	///////////////////////////////////////////////////

	// Static pointer to the main engine controller
	inline static Engine* engine = nullptr;

	// Static game_window ptr shared between ADK game objects
	static sf::RenderWindow* game_window_static;

	// Static pointer to ADKTimer Singleton. ADKGameStatics::timer should return same ADKTimer as ADKTimer::get_timer;
	static ADKTimer* timer;

	// Static pointer to currently active scene.
	static Scene* active_scene;

	////////////////////////////////////////////////////

};

