#pragma once
#include <SFML/Window/Window.hpp>

#pragma region INPUTS
/*
define enum of engine defined keys such as W, A, S, D, SPACE, X-Button, A-Button, Dpad-left
Then connect those keys to SFML equivalents

*/

#define KEY_ESCAPE sf::Keyboard::Escape
#define KEY_Q sf::Keyboard::Q
#define KEY_W sf::Keyboard::W
#define KEY_E sf::Keyboard::E
#define KEY_R sf::Keyboard::R
#define KEY_T sf::Keyboard::T
#define KEY_Y sf::Keyboard::Y
#define KEY_U sf::Keyboard::U
#define KEY_I sf::Keyboard::I
#define KEY_O sf::Keyboard::O
#define KEY_P sf::Keyboard::P
#define KEY_A sf::Keyboard::A
#define KEY_S sf::Keyboard::S
#define KEY_D sf::Keyboard::D
#define KEY_F sf::Keyboard::F
#define KEY_G sf::Keyboard::G
#define KEY_H sf::Keyboard::H
#define KEY_J sf::Keyboard::J
#define KEY_K sf::Keyboard::K
#define KEY_L sf::Keyboard::L
#define KEY_Z sf::Keyboard::Z
#define KEY_X sf::Keyboard::X
#define KEY_C sf::Keyboard::C

#pragma endregion


#pragma region ACTION AXIS BINDINGS
/*
define enum of actions/axis like Jump, Shoot, Move. Very UE4 input esque.
Then bind those actions/axis to engine defined keys

*/
#define ACTION_JUMP KEY_J;

#pragma endregion

namespace ADKAction
{
	enum action
	{
		JUMP,
		MOVELEFT,
		MOVERIGHT
	};
}

class ADKInput
{


public:

	/*
	key down / press
	key up / release
	key held down / poll constantly
	key held up / i.e. not pressed
	*/

	static void bind(ADKAction::action action_to_bind, sf::Keyboard::Key key);

	static bool is_key_pressed();

	//static bool is_;

	static void update_event(sf::Event& new_event) {  }

private:

	// map ADK::action to key

	static sf::Event& event_this_tick;

};