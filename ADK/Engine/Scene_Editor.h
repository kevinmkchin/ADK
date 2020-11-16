#pragma once

#include <imfilebrowser.h>

#include "Scene.h"

enum ToolMode
{
	TOOL_SELECTION,		// Select a placed entity
	TOOL_PLACE,			// Place a single entity
	TOOL_BRUSH,			// Place multiple entities
	TOOL_ERASER,
	TOOL_PICKER			// Picks entity type from level
};

struct FEditorConfig
{
public:
	int grid_size_x = 32;
	int grid_size_y = 32;
	sf::Color grid_color = sf::Color(255, 255, 255, 30);
	bool b_show_grid = true;
	bool b_snap_to_grid = false;

	int big_grid_x = 0;
	int big_grid_y = 0;
	sf::Color big_grid_color = sf::Color::Red;
	bool b_show_big_grid = false;

	sf::Color selection_color = sf::Color::White;

	sf::Vector2i top_left_pixel; // Top Left corner of the editor viewport in window pixel space
	sf::Vector2i bot_right_pixel; // Bottom Right corner of the editor viewport in window pixel space
	int window_size_x;
	int window_size_y;

	int depth_filter_lowerbound = -10;
	int depth_filter_upperbound = 10;
	// Tag filter?
};

class Scene_Editor : public Scene
{

public:
	Scene_Editor();

	virtual void begin_scene(sf::RenderWindow& window) override;

	virtual void end_scene(sf::RenderWindow& window) override;

	virtual void process_events(sf::Event& event) override;

	virtual void update_pre(float deltaTime) override;

	virtual void update(float deltaTime) override;

	virtual void update_post(float deltaTime) override;

	virtual void render_pre(sf::RenderWindow& window) override;

	virtual void render(sf::RenderWindow& window) override;

	virtual void render_post(sf::RenderWindow& window) override;

	virtual void show_scene_debugui() override;

protected:
	virtual void initialize_scene_view(sf::RenderWindow& window) override;

private:
	// ImGui
	void draw_editor_ui();
	void draw_menu_and_optionsbar_ui();
	void draw_tools_menu_ui();
	void draw_entity_type_ui();
	void draw_entity_property_ui();

	// Editor logic
	// USE THIS TO SET ENTITY SELECTED FOR PROPERTIES
	void set_entity_selected_for_properties(Entity* newSelection);

	// Update editor window positions and size (imgui windows)
	void update_editor_config_with_window(sf::RenderWindow& window);

	// Create and place a new entity at the brush location
	void brush_place_helper();

private:
	// Editor Configurations
	FEditorConfig default_editor_config;
	FEditorConfig active_editor_config;

	// List of available entities to create
	EntityList entity_types;

	// Editor window pointer
	sf::RenderWindow* render_window_ptr;
	
	// Entity that holds the EntityId of the Entity type to create
	Entity* entity_selected_for_creation;

	// Entity that is currently selected to edit properties or move around
	Entity* entity_selected_for_properties;

	// Represents the level viewport area
	sf::FloatRect bg_rect;

	// Logic stuff
	bool b_brush_enabled;
	bool b_entity_drag;
	sf::Vector2f entity_drag_offset;
	bool b_mouse_drag;
	sf::Vector2i last_mouse_pos;
	float zoom_factor;
	bool b_texture_show;
	ToolMode curr_tool;

	// Remember visited world positions while using brush so we don't create an entity in the same position
	std::vector<sf::Vector2i> brush_visited_positions;

	// Tool buttons textures
	sf::Texture select_button;
	sf::Texture place_button;
	sf::Texture brush_button;
	sf::Texture picker_button;

	// Texture loader file dialog
	ImGui::FileBrowser texture_dialog;

	// Local entity to store data of copied entity
	Entity copied_entity;
	float default_copy_paste_timer;
	float copy_paste_timer;

	// Config
	bool b_show_config;

	// Save & Load
	char level_id[30] = "";
	bool b_typing_level_id;

	// alt rotate
	bool b_alt_rotate;
	sf::Vector2f vec1;
	float og_rot;

	// ctrl scale
	bool b_shift_scale;
	sf::Vector2f og_mouse;
	float og_scale;

	// collision properties
	bool b_debug_render;
	bool b_collision_match_sprite_bound;

};

