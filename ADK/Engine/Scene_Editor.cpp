#include <iostream>
#include <fstream>
#include <filesystem>
#include <imgui.h>
#include <imgui-SFML.h>
#include "json.hpp"

#include "ADKMath.h"
#include "Scene_Editor.h"
#include "ADKSaveLoad.h"
#include "ADKAssets.h"

Scene_Editor::Scene_Editor()
	: default_editor_config(FEditorConfig())
	, active_editor_config(default_editor_config)
	, render_window_ptr(nullptr)
	, entity_selected_for_creation(nullptr)
	, entity_selected_for_properties(nullptr)
	, bg_rect(sf::FloatRect(0.03f * 1600, 0.04f * 900, 0.78f * 1600, 0.738f * 900))
	, b_brush_enabled(false)
	, b_entity_drag(false)
	, b_mouse_drag(false)
	, zoom_factor(1.f)
	, b_texture_show(false)
	, last_mouse_pos(sf::Vector2f())
	, curr_tool(TOOL_SELECTION)
	, default_copy_paste_timer(0.35f)
	, copy_paste_timer(default_copy_paste_timer)
	, b_show_config(false)
	, b_typing_in_textbox(false)
	, b_show_load_confirm(false)
	, b_show_save_confirm(false)
	, b_debug_render(false)
	, b_collision_match_sprite_bound(false)
	, b_prefab_for_creation(false)
{
}

void Scene_Editor::begin_scene(sf::RenderWindow& window)
{
	LOG("Began Editor Scene");

	// Load editor config
	std::ifstream config_load("Saved/Config/Editor.json");
	nlohmann::json config_json;
	config_load >> config_json;
	default_editor_config.grid_size_x = config_json["GridSizeX"];
	default_editor_config.grid_size_y = config_json["GridSizeY"];
	default_editor_config.b_show_grid = config_json["bShowGrid"];
	default_editor_config.b_snap_to_grid = config_json["bSnapGrid"];
	default_editor_config.grid_color.r = config_json["GridColorR"];
	default_editor_config.grid_color.g = config_json["GridColorG"];
	default_editor_config.grid_color.b = config_json["GridColorB"];
	default_editor_config.grid_color.a = config_json["GridColorA"];
	default_editor_config.selection_color.r = config_json["SelectColorR"];
	default_editor_config.selection_color.g = config_json["SelectColorG"];
	default_editor_config.selection_color.b = config_json["SelectColorB"];
	default_editor_config.selection_color.a = config_json["SelectColorA"];
	default_editor_config.depth_filter_lowerbound = config_json["DepthFilterLower"];
	default_editor_config.depth_filter_upperbound = config_json["DepthFilterUpper"];
	default_editor_config.big_grid_x = config_json["BigGridSizeX"];
	default_editor_config.big_grid_y = config_json["BigGridSizeY"];
	default_editor_config.b_show_big_grid = config_json["bShowBigGrid"];
	default_editor_config.big_grid_color.r = config_json["BigGridColorR"];
	default_editor_config.big_grid_color.g = config_json["BigGridColorG"];
	default_editor_config.big_grid_color.b = config_json["BigGridColorB"];
	default_editor_config.big_grid_color.a = config_json["BigGridColorA"];
	default_editor_config.collider_debug_color.r = config_json["ColliderDebugColorR"];
	default_editor_config.collider_debug_color.g = config_json["ColliderDebugColorG"];
	default_editor_config.collider_debug_color.b = config_json["ColliderDebugColorB"];
	default_editor_config.collider_debug_color.a = config_json["ColliderDebugColorA"];
	default_editor_config.editor_background_color.r = config_json["EditorBackgroundColorR"];
	default_editor_config.editor_background_color.g = config_json["EditorBackgroundColorG"];
	default_editor_config.editor_background_color.b = config_json["EditorBackgroundColorB"];
	default_editor_config.editor_background_color.a = config_json["EditorBackgroundColorA"];
	b_debug_render = config_json["bDebugRenderMode"];
	view_config.size_x = config_json["DefaultViewResolutionX"];
	view_config.size_y = config_json["DefaultViewResolutionY"];
	view_config.position_x = config_json["DefaultViewPositionX"];
	view_config.position_y = config_json["DefaultViewPositionY"];
	if (default_editor_config.big_grid_x == 0 || default_editor_config.big_grid_y == 0)
	{
		default_editor_config.big_grid_x = (int)view_config.size_x;
		default_editor_config.big_grid_y = (int)view_config.size_y;
	}
	level_entities.debug_color = active_editor_config.collider_debug_color;

	// Setup window values & active editor config
	active_editor_config = default_editor_config;
	update_editor_config_with_window(window);

	// Load button textures
	select_button.loadFromFile("Assets/adk/button_selection.png");
	place_button.loadFromFile("Assets/adk/button_place.png");
	brush_button.loadFromFile("Assets/adk/button_brush.png");
	picker_button.loadFromFile("Assets/adk/button_picker.png");

	render_window_ptr = &window;

	// Entity Types from Class Database
	std::map<std::string, ADKClassDescription*> db = ADKClassDatabase::get_database()->get_all_class_descriptions();
	for (auto& it : db)
	{
		ADKClassDescription* class_desc = it.second;
		Entity* created = class_desc->constructor();
		created->load_default_texture();
		entity_types.add(created);
	}

	// Prefabs from Prefabs Folder
	std::string prefabs_directory = "Saved/Prefabs/";
	namespace stdfs = std::filesystem;
	std::vector<std::string> filenames;
	const stdfs::directory_iterator end{};
	for (stdfs::directory_iterator iter{ prefabs_directory }; iter != end; ++iter)
	{
		if (stdfs::is_regular_file(*iter))	// http://en.cppreference.com/w/cpp/experimental/fs/is_regular_file 
		{
			filenames.push_back(iter->path().string());
		}
	}
	ADKSaveLoad PrefabLoader;
	for (std::size_t i = 0; i < filenames.size(); ++i)
	{
		EntityList* new_group = new EntityList;
		std::string name = filenames.at(i);
		name = name.substr(prefabs_directory.length());
		PrefabLoader.load_prefab_group(name.c_str(), new_group);
		prefab_groups.push_back(new_group);
	}

	// Initialize texture load file dialog
	texture_dialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	texture_dialog.SetTitle("Choose a texture to use");
	texture_dialog.SetTypeFilters({ ".png", ".jpg" });

	// Setup scene
	initialize_scene_view(window);
	window.setKeyRepeatEnabled(false);
}

void Scene_Editor::end_scene(sf::RenderWindow& window)
{
	// Deallocate entity memory

	for (int i = prefab_groups.size() - 1; i != -1; --i)
	{
		EntityList* to_delete = prefab_groups.at(i);
		to_delete->remove_and_destroy_all();
		delete to_delete;
	}
	prefab_groups.clear();

	entity_types.remove_and_destroy_all();

	level_entities.remove_and_destroy_all();

	Scene::end_scene(window);
}

void Scene_Editor::process_events(sf::Event& event)
{
	// Process events/inputs for ImGui::SFML
	ImGui::SFML::ProcessEvent(event);

	// Resized. Update Editor Config window values.
	if (event.type == sf::Event::Resized)
	{
		update_editor_config_with_window(*render_window_ptr);
	}

	// Shortcuts
	if (event.type == sf::Event::KeyPressed)
	{
		if (b_typing_in_textbox == false)
		{
			if (event.key.code == sf::Keyboard::F)
			{
				curr_tool = TOOL_SELECTION;
			}
			if (event.key.code == sf::Keyboard::D)
			{
				curr_tool = TOOL_PLACE;
			}
			if (event.key.code == sf::Keyboard::S)
			{
				curr_tool = TOOL_BRUSH;
			}
			if (event.key.code == sf::Keyboard::A)
			{
				curr_tool = TOOL_PICKER;
			}
			if (event.key.code == sf::Keyboard::R)
			{
				active_editor_config.b_show_grid = !active_editor_config.b_show_grid;
			}
			if (event.key.code == sf::Keyboard::T)
			{
				active_editor_config.b_snap_to_grid = !active_editor_config.b_snap_to_grid;
			}
		}

		if (event.key.code == sf::Keyboard::Delete)
		{
			if (entity_selected_for_properties != nullptr)
			{
				level_entities.remove_and_destroy(entity_selected_for_properties);
				set_entity_selected_for_properties(nullptr);
			}
		}
	}

	// Editor features that should only happen when sprite sheet viewer is closed.
	if (b_texture_show == false && b_show_config == false && b_show_load_confirm == false && b_show_save_confirm == false && texture_dialog.IsOpened() == false)
	{
		// Select entity
		if (curr_tool == TOOL_SELECTION && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
		{
			sf::IntRect viewWindow(active_editor_config.top_left_pixel.x, active_editor_config.top_left_pixel.y,
				active_editor_config.bot_right_pixel.x - active_editor_config.top_left_pixel.x, active_editor_config.bot_right_pixel.y - active_editor_config.top_left_pixel.y);
			for (int i = level_entities.size() - 1; i > -1; --i)
			{
				Entity* at = level_entities.at(i);
				sf::IntRect spr = at->get_sprite().getTextureRect();

				sf::FloatRect mouseCol = at->get_sprite().getGlobalBounds();
				sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
				sf::Vector2f worldPos = (*render_window_ptr).mapPixelToCoords(pixelPos);
				if (mouseCol.contains(worldPos) 
					&& at->get_depth() >= active_editor_config.depth_filter_lowerbound 
					&& at->get_depth() <= active_editor_config.depth_filter_upperbound 
					&& viewWindow.contains(pixelPos))
				{
					set_entity_selected_for_properties(at);
					if (entity_selected_for_properties == at) // This entity is already clicked on
					{
						b_entity_drag = true;
						sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
						sf::Vector2f worldPos = (*render_window_ptr).mapPixelToCoords(pixelPos);
						sf::Vector2f entPos = entity_selected_for_properties->get_position();
						entity_drag_offset = worldPos - entPos;
						return;
						//continue; // see if theres anything under it that we are trying to select
					}
					return; // don't do the rest of the inputs
				}
			}
		}

		// Place entity
		if (bg_rect.contains(sf::Vector2f(sf::Mouse::getPosition(*render_window_ptr))) && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
		{
			if (entity_selected_for_creation != nullptr)
			{
				// Single place entity
				if (curr_tool == TOOL_PLACE)
				{
					brush_place_helper();

					b_entity_drag = true;
				}

				// Brush place entity
				if (curr_tool == TOOL_BRUSH)
				{
					b_brush_enabled = true;
				}
			}
		}

		// Mouse view zoom
		if (bg_rect.contains(sf::Vector2f(sf::Mouse::getPosition(*render_window_ptr))) && b_mouse_drag == false && event.type == sf::Event::MouseWheelMoved)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				if (entity_selected_for_properties != nullptr)
				{
					if (event.mouseWheel.delta > 0)
					{
						entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame--;
						if ((int) entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame < 0)
						{
							entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame = 0;
						}
					}
					else
					{
						int width = (int) entity_selected_for_properties->get_sprite().getTexture()->getSize().x / entity_selected_for_properties->sprite_sheet.frame_size.x;
						int height = (int) entity_selected_for_properties->get_sprite().getTexture()->getSize().y / entity_selected_for_properties->sprite_sheet.frame_size.y;
						int max = width * height;
						entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame++;
						if ((int) entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame > max)
						{
							entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame = max;
						}
					}
				}
			}
			else
			{
				zoom_factor *= event.mouseWheel.delta > 0 ? 0.9f : 1.1f;
				sf::View curr_view = render_window_ptr->getView();
				curr_view.zoom(event.mouseWheel.delta > 0 ? 0.9f : 1.1f);
				render_window_ptr->setView(curr_view);
			}
		}
	
		// Ent rotate with Left Alt and Ctrl
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		{
			if (b_alt_rotate == false)
			{
				sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
				sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
				if (entity_selected_for_properties != nullptr)
				{
					sf::Vector2f ePos = entity_selected_for_properties->get_position();
					vec1 = mousePos - ePos;
					og_rot = entity_selected_for_properties->get_rotation();
				}
			}
			b_alt_rotate = true;
		}
		else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::LAlt || event.key.code == sf::Keyboard::LControl))
		{
			b_alt_rotate = false;
		}

		// Ent scale with Left Ctrl and Shift
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		{
			if (b_shift_scale == false)
			{
				sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
				og_mouse = (*render_window_ptr).mapPixelToCoords(pixelPos);
				if (entity_selected_for_properties != nullptr)
				{
					og_scale = entity_selected_for_properties->get_scale();
				}
			}
			b_shift_scale = true;
		}
		else if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::LControl))
		{
			b_shift_scale = false;
		}

		// Copy
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			if (entity_selected_for_properties != nullptr)
			{
				copied_entity = entity_selected_for_properties;
			}
		}

		// Paste
		if (copy_paste_timer < 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::V))
		{
			// Get the current mouse position in the window
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
			// Convert it to world coordinates
			sf::Vector2f worldPos = (*render_window_ptr).mapPixelToCoords(pixelPos);
			// Calculate amount to subtract if snapping to grid
			int sX = 0;
			int sY = 0;
			if (active_editor_config.b_snap_to_grid)
			{
				sX = (int)worldPos.x % active_editor_config.grid_size_x;
				if (worldPos.x < 0)
				{
					sX = active_editor_config.grid_size_x + sX;
				}
				sY = (int)worldPos.y % active_editor_config.grid_size_y;
				if (worldPos.y < 0)
				{
					sY = active_editor_config.grid_size_y + sY;
				}
			}

			// Get would-be position of a new entity
			int posX = ((int)worldPos.x) - sX;
			int posY = ((int)worldPos.y) - sY;

			// Create a new entity
			Entity* created = copied_entity->class_description_ptr->constructor();
			Entity::copy(*created, *copied_entity);
			created->set_position((float)posX, (float)posY);

			// Add the entity to this scene/level editor's entity list
			level_entities.add(created);
			level_entities.mark_depth_changed();
			// Set this entity to be selected
			set_entity_selected_for_properties(created);

			copy_paste_timer = default_copy_paste_timer;
		}
		if (event.type == sf::Event::KeyReleased && (event.key.code == sf::Keyboard::LControl || event.key.code == sf::Keyboard::V))
		{
			copy_paste_timer = -1.f;
		}
	}

	// Left mouse released (Stop entity drag & disable brush placement & empty brush visited positions
	if (event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonReleased)
	{
		b_entity_drag = false;
		entity_drag_offset = sf::Vector2f(0.f,0.f);
		// Empty the brush tools visited positions cache
		b_brush_enabled = false;
		if (brush_visited_positions.empty() == false)
		{
			brush_visited_positions.clear();
		}
	}

	// Mouse view drag
	if (event.mouseButton.button == sf::Mouse::Middle)
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{

			// Set last mouse pos to current mouse pos
			last_mouse_pos = sf::Mouse::getPosition(*render_window_ptr);
			b_mouse_drag = true;
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			b_mouse_drag = false;
		}
	}

	// Scene move with Arrows
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) == false)
	{
		float xM = 0;
		float yM = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			xM -= 10;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			xM += 10;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			yM -= 10;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			yM += 10;
		}
		sf::View curr_view = render_window_ptr->getView();
		curr_view.move(sf::Vector2f(xM, yM));
		render_window_ptr->setView(curr_view);
	}

	// Entity move with Arrows
	if (entity_selected_for_properties != nullptr && b_typing_in_textbox == false 
		&& sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) == false)
	{
		float xM = 0;
		float yM = 0;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			xM -= 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			xM += 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			yM -= 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			yM += 1;
		}
		sf::Vector2f pos = entity_selected_for_properties->get_position();
		pos.x += xM;
		pos.y += yM;
		entity_selected_for_properties->set_position(pos);
	}
}

void Scene_Editor::update_pre(float deltaTime)
{

}

void Scene_Editor::update(float deltaTime)
{
	level_entities.update_animation_only(deltaTime);

	// Call brush place
	if (curr_tool == TOOL_BRUSH && b_brush_enabled)
	{
		brush_place_helper();
	}
	else
	{
		b_brush_enabled = false;
	}

	// Move entity if dragging it around
	if (b_entity_drag && entity_selected_for_properties != nullptr)
	{
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
		sf::Vector2f worldPos = (*render_window_ptr).mapPixelToCoords(pixelPos);
		// Calculate amount to subtract if snapping to grid
		int sX = 0;
		int sY = 0;
		if (active_editor_config.b_snap_to_grid)
		{
			int gridSubX = (int) entity_drag_offset.x / active_editor_config.grid_size_x;
			int gridSubY = (int) entity_drag_offset.y / active_editor_config.grid_size_y;
			worldPos.x -= gridSubX * active_editor_config.grid_size_x;
			worldPos.y -= gridSubY * active_editor_config.grid_size_y;
			sX = (int)worldPos.x % active_editor_config.grid_size_x;
			if (worldPos.x < 0)
			{
				sX = active_editor_config.grid_size_x + sX;
			}
			sY = (int)worldPos.y % active_editor_config.grid_size_y;
			if (worldPos.y < 0)
			{
				sY = active_editor_config.grid_size_y + sY;
			}
		}
		else
		{
			worldPos -= entity_drag_offset;
		}
		// Set entity's position
		entity_selected_for_properties->set_position((float)((int)worldPos.x - sX), (float)((int)worldPos.y - sY));
	}

	// Move view if dragging view
	if (b_mouse_drag)
	{		
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
		sf::Vector2f delta = sf::Vector2f(pixelPos - last_mouse_pos);
		float res_factor = view_config.size_x / (default_editor_config.bot_right_pixel.x - default_editor_config.top_left_pixel.x);
		sf::View curr_view = render_window_ptr->getView();
		curr_view.move(-delta * res_factor * zoom_factor);
		render_window_ptr->setView(curr_view);
		last_mouse_pos = pixelPos;
	}

	// Alt Rotate
	if (b_alt_rotate && entity_selected_for_properties != nullptr)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			entity_selected_for_properties->set_rotation(0.f, b_collision_match_sprite_bound);
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
			sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
			if (entity_selected_for_properties != nullptr)
			{
				sf::Vector2f ePos = entity_selected_for_properties->get_position();
				vec1 = mousePos - ePos;
				og_rot = entity_selected_for_properties->get_rotation();
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			entity_selected_for_properties->set_rotation(90.f, b_collision_match_sprite_bound);
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
			sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
			if (entity_selected_for_properties != nullptr)
			{
				sf::Vector2f ePos = entity_selected_for_properties->get_position();
				vec1 = mousePos - ePos;
				og_rot = entity_selected_for_properties->get_rotation();
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			entity_selected_for_properties->set_rotation(180.f, b_collision_match_sprite_bound);
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
			sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
			if (entity_selected_for_properties != nullptr)
			{
				sf::Vector2f ePos = entity_selected_for_properties->get_position();
				vec1 = mousePos - ePos;
				og_rot = entity_selected_for_properties->get_rotation();
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			entity_selected_for_properties->set_rotation(270.f, b_collision_match_sprite_bound);
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
			sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
			if (entity_selected_for_properties != nullptr)
			{
				sf::Vector2f ePos = entity_selected_for_properties->get_position();
				vec1 = mousePos - ePos;
				og_rot = entity_selected_for_properties->get_rotation();
			}
		}
		else
		{
			sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
			sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
			sf::Vector2f vec2 = mousePos - entity_selected_for_properties->get_position();
			float angle = ADKMath::get_angle_between_vectors(vec1, vec2);
			entity_selected_for_properties->set_rotation(og_rot + angle, b_collision_match_sprite_bound);
		}
	}

	// Ctrl scale
	if (b_shift_scale && entity_selected_for_properties != nullptr)
	{
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
		sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
		sf::Vector2f diff = mousePos - og_mouse;

		// see if negative
		sf::Vector2f toMouse = mousePos - entity_selected_for_properties->get_position();
		float mousefromEnt = std::sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
		sf::Vector2f toOg = og_mouse - entity_selected_for_properties->get_position();
		float ogfromEnt = std::sqrt(toOg.x * toOg.x + toOg.y * toOg.y);

		float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
		float scale = ((len * (mousefromEnt > ogfromEnt ? 1 : -1)) + 280.f) / 280.f;

		entity_selected_for_properties->set_scale(og_scale * scale, true);
	}

	// Decrement copy paste timer
	if (copy_paste_timer > 0.f)
	{
		copy_paste_timer -= deltaTime;
	}

	b_typing_in_textbox = false;
	// Setup ImGui to draw
	draw_editor_ui();
}

void Scene_Editor::update_post(float deltaTime)
{

}

void Scene_Editor::render_pre(sf::RenderWindow& window)
{
	window.clear(active_editor_config.editor_background_color);
}

void Scene_Editor::render(sf::RenderWindow& window)
{
	level_entities.render_with_depth(window, active_editor_config.depth_filter_lowerbound, active_editor_config.depth_filter_upperbound);
	if (b_debug_render)
	{
		level_entities.render_with_depth(window, active_editor_config.depth_filter_lowerbound, active_editor_config.depth_filter_upperbound, true);
	}

	// Render Grid
	if (active_editor_config.b_show_grid)
	{
		sf::Vector2f topLeftWorld = (*render_window_ptr).mapPixelToCoords(active_editor_config.top_left_pixel);
		sf::Vector2f botRightWorld = (*render_window_ptr).mapPixelToCoords(active_editor_config.bot_right_pixel);

		int gridRangeX = 6400 / active_editor_config.grid_size_x; 
		int gridRangeY = 6400 / active_editor_config.grid_size_y; 

		for (int x = -gridRangeX; x < gridRangeX; ++x)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f((float) (x * active_editor_config.grid_size_x), topLeftWorld.y);
			line[0].color = active_editor_config.grid_color;
			line[1].position = sf::Vector2f((float) (x * active_editor_config.grid_size_x), botRightWorld.y);
			line[1].color = active_editor_config.grid_color;
			window.draw(line, 2, sf::Lines);
		}

		for (int y = -gridRangeY; y < gridRangeY; ++y)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f(topLeftWorld.x, (float) (y * active_editor_config.grid_size_y));
			line[0].color = active_editor_config.grid_color;
			line[1].position = sf::Vector2f(botRightWorld.x, (float) (y * active_editor_config.grid_size_y));
			line[1].color = active_editor_config.grid_color;
			window.draw(line, 2, sf::Lines);
		}
	}

	// Render big grid
	if (active_editor_config.b_show_big_grid)
	{
		sf::Vector2f topLeftWorld = (*render_window_ptr).mapPixelToCoords(active_editor_config.top_left_pixel);
		sf::Vector2f botRightWorld = (*render_window_ptr).mapPixelToCoords(active_editor_config.bot_right_pixel);

		int gridRangeX = 6400 / active_editor_config.big_grid_x;
		int gridRangeY = 6400 / active_editor_config.big_grid_y;

		for (int x = -gridRangeX; x < gridRangeX; ++x)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f((float)(x * active_editor_config.big_grid_x), topLeftWorld.y);
			line[0].color = active_editor_config.big_grid_color;
			line[1].position = sf::Vector2f((float)(x * active_editor_config.big_grid_x), botRightWorld.y);
			line[1].color = active_editor_config.big_grid_color;
			window.draw(line, 2, sf::Lines);
		}

		for (int y = -gridRangeY; y < gridRangeY; ++y)
		{
			sf::Vertex line[2];
			line[0].position = sf::Vector2f(topLeftWorld.x, (float)(y * active_editor_config.big_grid_y));
			line[0].color = active_editor_config.big_grid_color;
			line[1].position = sf::Vector2f(botRightWorld.x, (float)(y * active_editor_config.big_grid_y));
			line[1].color = active_editor_config.big_grid_color;
			window.draw(line, 2, sf::Lines);
		}
	}

	// Show Selection
	if (entity_selected_for_properties != nullptr)
	{
		sf::FloatRect bounds = entity_selected_for_properties->get_sprite().getGlobalBounds();
		float x = bounds.left;
		float y = bounds.top;
		float width = bounds.width;
		float height = bounds.height;

		sf::Vertex box[8];
		box[0].position = sf::Vector2f(x, y);
		box[1].position = sf::Vector2f(x + width, y);
		box[2].position = sf::Vector2f(x + width, y + height);
		box[3].position = sf::Vector2f(x, y + height);
		box[4].position = sf::Vector2f(x, y);
		box[5].position = sf::Vector2f(x, y + height);
		box[6].position = sf::Vector2f(x + width, y + height);
		box[7].position = sf::Vector2f(x + width, y);
		for (int i = 0; i < 8; ++i)
		{
			box[i].color = active_editor_config.selection_color;
		}
		window.draw(box, 8, sf::Lines);

		// Selection box is tinted
		//sf::RectangleShape selection;
		//selection.setRotation(EntitySelectedForProperties->GetRotation());
		//sf::Color selectionColor = ActiveEditorConfig.SelectionColor;
		//selectionColor.a /= 5;
		//selection.setFillColor(selectionColor);
		//selection.setPosition(x, y);
		//selection.setSize(sf::Vector2f(width, height));
		//window.draw(selection);
	}
}

void Scene_Editor::render_post(sf::RenderWindow& window)
{

}

void Scene_Editor::show_scene_debugui()
{
	ImGui::Text("Editor: Load Level");

	std::string levels_directory = "Assets/Levels/";
	namespace stdfs = std::filesystem;
	std::vector<std::string> filenames;
	const stdfs::directory_iterator end{};
	for (stdfs::directory_iterator iter{ levels_directory }; iter != end; ++iter)
	{
		if (stdfs::is_regular_file(*iter))	// http://en.cppreference.com/w/cpp/experimental/fs/is_regular_file 
		{
			filenames.push_back(iter->path().string());
		}
	}
	
	for (std::size_t i = 0; i < filenames.size(); ++i)
	{
		ImGui::PushID(i);
		std::string name = filenames.at(i);
		name = name.substr(levels_directory.length());
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			load_level_confirmation(name);
		}
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			save_level_confirmation(name);
		}
		ImGui::PopID();
	}
}

void Scene_Editor::load_level_confirmation(std::string path)
{
	b_show_load_confirm = true;
	b_show_save_confirm = false;
	pending_level_path = path;
}

void Scene_Editor::save_level_confirmation(std::string path)
{
	b_show_save_confirm = true;
	b_show_load_confirm = false;
	pending_level_path = path;
}

void Scene_Editor::draw_editor_ui()
{
	// Save Load popup
	if (b_show_save_confirm)
	{
		ImGui::Begin("Save Confirmation");
		std::string confirm_text = "Attempting to save/overwrite '" + pending_level_path + "'. Proceed with save?";
		ImGui::Text(confirm_text.c_str());
		if (ImGui::Button("yes"))
		{
			ADKSaveLoad Saver;
			Saver.save_scene(pending_level_path, *this);
			b_show_save_confirm = false;
		}
		if (ImGui::Button("no"))
		{
			b_show_save_confirm = false;
		}
		ImGui::End();
	}
	if (b_show_load_confirm)
	{
		ImGui::Begin("Load Confirmation");
		std::string confirm_text = "Attempting to load '" + pending_level_path + "'. Proceed with load?";
		ImGui::Text(confirm_text.c_str());
		if (ImGui::Button("yes"))
		{
			entity_selected_for_properties = nullptr;
			entity_selected_for_creation = nullptr;

			// delete all entities in scene viewer
			for (int i = level_entities.size() - 1; i != -1; --i)
			{
				level_entities.remove_and_destroy(level_entities.at(i));
			}

			ADKSaveLoad Loader;
			Loader.load_to_scene(pending_level_path, *this);
			b_show_load_confirm = false;
		}
		if (ImGui::Button("no"))
		{
			b_show_load_confirm = false;
		}
		ImGui::End();
	}

	// Actual editor draw
	draw_menu_and_optionsbar_ui();
	draw_entity_property_ui();
	draw_entity_type_ui();
	draw_tools_menu_ui();
}

void Scene_Editor::draw_entity_property_ui()
{
	ImGui::Begin("Entities and Properties", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f((float) active_editor_config.bot_right_pixel.x, (float) active_editor_config.top_left_pixel.y));
	ImGui::SetWindowSize(sf::Vector2f((float) active_editor_config.window_size_x - active_editor_config.bot_right_pixel.x, (float) active_editor_config.window_size_y - active_editor_config.top_left_pixel.y));

	ImGui::BeginTabBar("");
	// Selected Entity Properties
	if (ImGui::BeginTabItem("Properties"))
	{
		// Display properties for currently selected entity
		if (entity_selected_for_properties != nullptr)
		{
#pragma region EntityProperties
			ImGui::Separator();
			ImGui::Text("Entity Properties");
			ImGui::Separator();

			int x = (int) entity_selected_for_properties->get_position().x;
			int y = (int) entity_selected_for_properties->get_position().y;
			ImGui::InputInt("X Position", &x);
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->set_position((float)x, (float)y);
			}
			ImGui::InputInt("Y Position", &y);
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->set_position((float)x, (float)y);
			}

			float angle = entity_selected_for_properties->get_rotation();
			ImGui::InputFloat("Rotation", &angle);
			if (ImGui::IsItemActive())
			{
				entity_selected_for_properties->set_rotation(angle, b_collision_match_sprite_bound);
			}
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->set_rotation(angle, b_collision_match_sprite_bound);
			}

			float scale_og = entity_selected_for_properties->get_scale();
			float scale = scale_og;
			ImGui::InputFloat("Scale", &scale);
			if (ImGui::IsItemDeactivated() && scale != scale_og)
			{
				entity_selected_for_properties->set_scale(scale, true);
			}

			int depth = entity_selected_for_properties->get_depth();
			ImGui::InputInt("Depth", &depth);
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->set_depth(depth);
			}

			bool v = entity_selected_for_properties->is_visible();
			ImGui::Checkbox("Visible", &v);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Whether to Render this entity");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->set_visible(v);
			}

			bool a = entity_selected_for_properties->is_active();
			ImGui::Checkbox("Active", &a);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Whether to Update this entity");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->set_active(a);
			}

			ADKClassDescription* class_desc = entity_selected_for_properties->class_description_ptr;
			if (class_desc->fields.empty() == false)
			{
				ImGui::Separator();
				std::string text1 = class_desc->type.name.text + " Properties";
				ImGui::Text(text1.c_str());
				ImGui::Separator();
			}

			ImGui::PushItemWidth(100.f);
			// ENTITY FIELDS
			std::vector<ADKFieldDescription> fields = class_desc->fields;
			for (ADKFieldDescription field : fields)
			{
				std::string fieldtype = field.type.name.text;
				if (fieldtype == typeid(int).name())
				{
					char* ent_address = (char*)entity_selected_for_properties;
					int* int_address = (int*)(ent_address + field.offset);
					ImGui::InputInt(field.name.text.c_str(), int_address);
				}
				else if (fieldtype == typeid(float).name())
				{
					char* ent_address = (char*)entity_selected_for_properties;
					float* float_address = (float*)(ent_address + field.offset);
					ImGui::InputFloat(field.name.text.c_str(), float_address);
				}
				else if (fieldtype == typeid(bool).name())
				{
					char* ent_address = (char*)entity_selected_for_properties;
					bool* bool_address = (bool*)(ent_address + field.offset);
					ImGui::Checkbox(field.name.text.c_str(), bool_address);
				}
				else if (fieldtype == typeid(std::string).name())
				{

				}
			}

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region SpriteSheetProperties
			ImGui::Separator();
			ImGui::Text("SpriteSheet Properties");
			ImGui::Separator();

			// Choose Texture
			std::string sinfo = "Current sprite texture: " + entity_selected_for_properties->get_texture_path();
			ImGui::TextWrapped(sinfo.c_str());
			if (ImGui::Button("Change Texture"))
			{
				texture_dialog.Open();
			}
			texture_dialog.Display();
			if (texture_dialog.HasSelected())
			{
				if (texture_dialog.GetSelected().filename().has_extension())
				{
					size_t pos = texture_dialog.GetSelected().string().find("\\Assets\\");
					if (pos < 1000)
					{
						std::string path = texture_dialog.GetSelected().string().substr(pos).substr(7);
						entity_selected_for_properties->set_texture_path_and_load(path);
					}
				}
				texture_dialog.ClearSelected();
			}

			int si[2];
			si[0] = entity_selected_for_properties->sprite_sheet.frame_size.x;
			si[1] = entity_selected_for_properties->sprite_sheet.frame_size.y;

			ImGui::InputInt2("Frame Size", si);
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				entity_selected_for_properties->match_framesize_to_texture();
				si[0] = entity_selected_for_properties->sprite_sheet.frame_size.x;
				si[1] = entity_selected_for_properties->sprite_sheet.frame_size.y;
			}
			entity_selected_for_properties->sprite_sheet.frame_size.x = si[0];
			entity_selected_for_properties->sprite_sheet.frame_size.y = si[1];

			if (ImGui::Button("View Sprite Sheet"))
			{
				b_texture_show = !b_texture_show;
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("View the sprite sheet sliced by the specified frame size.");
				ImGui::TextUnformatted("Choose a frame from the sprite sheet to use as the starting frame of the currently selected animation. Tools aside from selection tool cannot be used while this window is open.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (b_texture_show)
			{
				ImGui::Begin("Currently selected texture", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowPos(sf::Vector2f(active_editor_config.bot_right_pixel.x - 610.f, 50.f));
				ImGui::SetWindowSize(sf::Vector2f(600.f, 600.f));

				ImGui::Text("Clicking on a frame will set that frame as the start frame of the currently selected animation.");
				ImGui::Text("Shift + Clicking on a frame will set that frame as the end frame of the currently selected animation.");

				sf::Vector2f textureBounds = sf::Vector2f(entity_selected_for_properties->get_sprite().getTexture()->getSize());
				int numWide = (int) textureBounds.x / ((si[0] > 0) ? si[0] : 1);
				int numTall = (int) textureBounds.y / ((si[1] > 0) ? si[1] : 1);
				float buttonSize = 500.f / numWide < 500.f / numTall ? 500.f / numWide : 500.f / numTall;
				for (int i = 0; i < numTall; ++i)
				{
					for (int j = 0; j < numWide; ++j)
					{
						ImGui::PushID((i * numWide) + j);

						sf::Sprite frame = entity_selected_for_properties->get_sprite();
						frame.setTextureRect(sf::IntRect(j * si[0], i * si[1], si[0], si[1]));

						if (ImGui::ImageButton(frame, sf::Vector2f(buttonSize, buttonSize)))
						{
							if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
							{
								if (entity_selected_for_properties->sprite_sheet.selected_animation >= 0)
								{
									// Set num frames
									int index = (i * numWide) + j;
									entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].num_frames
										= index - entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame + 1;
								}
							}
							else
							{
								if (entity_selected_for_properties->sprite_sheet.selected_animation >= 0)
								{
									// Set start frame
									int index = (i * numWide) + j;
									entity_selected_for_properties->sprite_sheet.animations[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame = index;
								}
							}
						}

						if (j != numWide - 1)
						{
							ImGui::SameLine();
						}

						ImGui::PopID();
					}
				}

				ImGui::End();
			}

			ImGui::Checkbox("Loop Animation", &entity_selected_for_properties->sprite_sheet.b_repeat);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Whether this animation loop or freeze at the last frame.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->sprite_sheet.current_frame
					= entity_selected_for_properties->sprite_sheet.animations
					[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame;
			}

			int selected = static_cast<int>(entity_selected_for_properties->sprite_sheet.selected_animation);
			ImGui::SliderInt("Anim Index", &selected, 0, entity_selected_for_properties->sprite_sheet.animations.size() - 1);
			entity_selected_for_properties->sprite_sheet.selected_animation = selected;
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Select Animation Index");
				ImGui::TextUnformatted("Select the index of the animation you want to use.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemDeactivatedAfterEdit())
			{
				entity_selected_for_properties->sprite_sheet.current_frame
					= entity_selected_for_properties->sprite_sheet.animations
					[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame;
			}

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region AnimationProperties
			ImGui::Separator();
			ImGui::Text("Animations");			
			ImGui::Separator();

			if (ImGui::Button("Add new animation"))
			{
				entity_selected_for_properties->sprite_sheet.animations.push_back(FAnimation());
			}

			// display animations
			for (size_t i = 0; i < entity_selected_for_properties->sprite_sheet.animations.size(); ++i)
			{
				ImGui::Text(std::to_string(i).c_str());
				ImGui::Indent();
				ImGui::PushID(i);
				
				float dur = entity_selected_for_properties->sprite_sheet.animations[i].anim_duration.asSeconds();
				ImGui::InputFloat("Duration", &dur);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("Duration of this animation in seconds. Set to 0 if you want a static sprite.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				entity_selected_for_properties->sprite_sheet.animations[i].anim_duration = sf::seconds(dur);
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					entity_selected_for_properties->sprite_sheet.current_frame
						= entity_selected_for_properties->sprite_sheet.animations
						[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame;
				}

				int sframe = static_cast<int>(entity_selected_for_properties->sprite_sheet.animations[i].start_frame);
				ImGui::InputInt("StartFrame", &sframe);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("Index of the frame that the animation starts on.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				entity_selected_for_properties->sprite_sheet.animations[i].start_frame = sframe;
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					entity_selected_for_properties->sprite_sheet.current_frame
						= entity_selected_for_properties->sprite_sheet.animations
						[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame;
				}

				int numFrames = static_cast<int>(entity_selected_for_properties->sprite_sheet.animations[i].num_frames);
				ImGui::InputInt("# of Frames", &numFrames);				
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("Number of frames for this animation, starting at StartFrame.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				entity_selected_for_properties->sprite_sheet.animations[i].num_frames = numFrames;
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					entity_selected_for_properties->sprite_sheet.current_frame
						= entity_selected_for_properties->sprite_sheet.animations
						[entity_selected_for_properties->sprite_sheet.selected_animation].start_frame;
				}

				if (ImGui::Button("Delete this animation"))
				{
					std::vector<FAnimation>::iterator nth = entity_selected_for_properties->sprite_sheet.animations.begin() + i;
					entity_selected_for_properties->sprite_sheet.animations.erase(nth);
					--i;

					// Make sure selected animation index isn't out of bounds
					if (entity_selected_for_properties->sprite_sheet.animations.size() >= entity_selected_for_properties->sprite_sheet.selected_animation)
					{
						--(entity_selected_for_properties->sprite_sheet.selected_animation);
					}
				}

				ImGui::PopID();
				ImGui::Unindent();
			}
			ImGui::PopItemWidth();

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region CollisionProperties
			ImGui::Separator();
			ImGui::Text("Collision Properties");
			ImGui::Separator();

			ImGui::PushItemWidth(120.f);
			ImGui::Checkbox("Match sprite bounds", &b_collision_match_sprite_bound);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Ignore x y offsets and width and height in order to force the box collider to match the global bounds of this sprite.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			if (b_collision_match_sprite_bound == false)
			{
				BoxCollider col = entity_selected_for_properties->get_collider();

				float off[2] = { col.offset_x, col.offset_y };
				ImGui::InputFloat2("x-y offsets", off);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("x, y offsets from the x, y positions of the sprite.");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				entity_selected_for_properties->get_collider().offset_x = off[0];
				entity_selected_for_properties->get_collider().offset_y = off[1];

				float siz[2] = { col.width, col.height };
				ImGui::InputFloat2("width & height", siz);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
					ImGui::TextUnformatted("width and height of the collider");
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				entity_selected_for_properties->get_collider().width = siz[0];
				entity_selected_for_properties->get_collider().height = siz[1];

				bool bCollidable = entity_selected_for_properties->is_collidable();
				ImGui::Checkbox("Collision Enabled", &bCollidable);
				entity_selected_for_properties->set_collidable(bCollidable);

			}
			ImGui::PopItemWidth();
#pragma endregion

			// Save to project prefabs
			ImGui::Separator();
			ImGui::Text("Prefab");
			ImGui::Separator();
			ImGui::InputText("Group ID", entity_selected_for_properties->prefab_group, 30);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Prefab Group ID is the name of the json file. A clever use might "
					"be to group all similar prefabs together, so that you end up with multiple prefab "
					"groups you can add and remove from different projects.");
				ImGui::TextUnformatted("Save path in /Saved/Prefabs/.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemActive())
			{
				b_typing_in_textbox = true;
			}
			ImGui::InputText("Prefab ID", entity_selected_for_properties->prefab_id, 30);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
				ImGui::TextUnformatted("Prefab ID is identifier of this prefab type.");
				ImGui::TextUnformatted("Save path in /Saved/Prefabs/.");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemActive())
			{
				b_typing_in_textbox = true;
			}
			if (ImGui::Button("Save Prefab"))
			{
				ADKSaveLoad PrefabSaver;
				PrefabSaver.save_prefab(entity_selected_for_properties->prefab_group,
					entity_selected_for_properties->prefab_id, entity_selected_for_properties);
			}
			if (entity_selected_for_properties->prefab_id[0] != 0)
			{
				std::string pinfo = "This prefab is based on type: " + entity_selected_for_properties->class_description_ptr->type.name.text;
				ImGui::TextWrapped(pinfo.c_str());
			}
		}

		ImGui::EndTabItem();
	}
	// Entities in Level
	if (ImGui::BeginTabItem("Entities in Level"))
	{
		// Two columns
		ImGui::Columns(2);

		// Display all entities currently in level
		int selected = -1;
		std::vector<std::string> bruh;
		for (int i = 0; i < level_entities.size(); ++i)
		{
			std::string name = std::to_string(i).append(" ");
			name = level_entities.at(i)->class_description_ptr->type.name.text;
			bruh.push_back(name);
			if (level_entities.at(i) == entity_selected_for_properties)
			{
				selected = i;
			}
		}
		if (ImGui::ListBox("", &selected, bruh))
		{
			set_entity_selected_for_properties(level_entities.at(selected));
		}
		ImGui::NextColumn();
		
		// Tags options
		ImGui::Text("Show Tags");
		bool ex = true;
		ImGui::Checkbox("Collider", &ex);
		bool ex2 = true;
		ImGui::Checkbox("Enemies", &ex2);
		bool ex3 = true;
		ImGui::Checkbox("Walls", &ex3);
		bool ex4 = true;
		ImGui::Checkbox("Background", &ex4);
		ImGui::Dummy(ImVec2(0.0f, 20.0f));
		ImGui::Text("Only show entities");
		ImGui::Text("with these tags in");
		ImGui::Text("the level viewer?");
		bool bShowOnlyTags;
		ImGui::Checkbox("", &bShowOnlyTags);
		ImGui::NextColumn();

		// Count of entities in level
		std::string num = std::to_string(level_entities.size()).append(" entities in level.");
		const char * numInCharPtr = num.c_str();
		ImGui::Text(numInCharPtr);

		// Delete Button
		if (ImGui::Button("Delete Selected"))
		{
			if (entity_selected_for_properties != nullptr)
			{
				level_entities.remove_and_destroy(entity_selected_for_properties);
				set_entity_selected_for_properties(nullptr);
			}
		}

		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();

	ImGui::End();
}

void Scene_Editor::draw_entity_type_ui()
{
	ImGui::Begin("Entity Types", nullptr, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowPos(sf::Vector2f(0.f, (float) active_editor_config.bot_right_pixel.y));
	ImGui::SetWindowSize(sf::Vector2f((float) active_editor_config.bot_right_pixel.x, (float) active_editor_config.window_size_y - active_editor_config.bot_right_pixel.y));

	ImGui::BeginTabBar("diff entity types bar id");
	// Base Entities Tab
	if (ImGui::BeginTabItem("Base Entities"))
	{
		// Display all available entity types
		for (int i = 0; i < entity_types.size(); ++i)
		{
			ImGui::BeginGroup();
			{
				ImGui::PushID(i);

				Entity* entity_type = entity_types.at(i);
				sf::Sprite entitySprite = entity_type->get_sprite();

				// TODO ADK find texture rect for these type entities only once
				sf::IntRect display_rect;
				sf::Vector2i textureBounds(entitySprite.getTexture()->getSize());
				int fsx = entity_type->sprite_sheet.frame_size.x;
				int fsy = entity_type->sprite_sheet.frame_size.y;
				int numFramesWide = textureBounds.x / ((fsx > 0) ? fsx : 1);
				int numFramesTall = textureBounds.y / ((fsy > 0) ? fsy : 1);
				int xLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame % (numFramesWide > 0 ? numFramesWide : 1)) * fsx;
				int yLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame / (numFramesWide > 0 ? numFramesWide : 1)) * fsy;
				display_rect.left = xLoc;
				display_rect.top = yLoc;
				display_rect.width = entitySprite.getTextureRect().width;
				display_rect.height = entitySprite.getTextureRect().height;
				entitySprite.setTextureRect(display_rect);

				const char* EntityId = entity_type->class_description_ptr->type.name.text.c_str();
				if (ImGui::ImageButton(entitySprite, sf::Vector2f(40.f, 40.f)))
				{
					entity_selected_for_creation = entity_types.at(i);
					b_prefab_for_creation = false;
					curr_tool = TOOL_PLACE;
				}
				ImGui::Text(EntityId);
				ImGui::PopID();

				++i;
				if (i < entity_types.size())
				{
					ImGui::PushID(i);

					entity_type = entity_types.at(i);
					entitySprite = entity_type->get_sprite();

					display_rect;
					sf::Vector2i textureBounds(entitySprite.getTexture()->getSize());
					fsx = entity_type->sprite_sheet.frame_size.x;
					fsy = entity_type->sprite_sheet.frame_size.y;
					numFramesWide = textureBounds.x / ((fsx > 0) ? fsx : 1);
					numFramesTall = textureBounds.y / ((fsy > 0) ? fsy : 1);
					xLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame % (numFramesWide > 0 ? numFramesWide : 1)) * fsx;
					yLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame / (numFramesWide > 0 ? numFramesWide : 1)) * fsy;
					display_rect.left = xLoc;
					display_rect.top = yLoc;
					display_rect.width = entitySprite.getTextureRect().width;
					display_rect.height = entitySprite.getTextureRect().height;
					entitySprite.setTextureRect(display_rect);

					EntityId = entity_type->class_description_ptr->type.name.text.c_str();
					if (ImGui::ImageButton(entitySprite, sf::Vector2f(40.f, 40.f)))
					{
						entity_selected_for_creation = entity_types.at(i);
						b_prefab_for_creation = false;
						curr_tool = TOOL_PLACE;
					}
					ImGui::Text(EntityId);
					ImGui::PopID();
				}
			}
			ImGui::EndGroup();
			ImGui::SameLine();
		}

		ImGui::EndTabItem();
	}
	// Prefabs Tab
	if (ImGui::BeginTabItem("Prefabs"))
	{
		// Load Prefabs saved to Project Prefabs

		ImGui::BeginTabBar("diff prefab groups bar id");

		for (EntityList* prefabs : prefab_groups)
		{
			if (ImGui::BeginTabItem(prefabs->at(0)->prefab_group))
			{
				for (int i = 0; i < prefabs->size(); ++i)
				{
					ImGui::BeginGroup();
					{
						ImGui::PushID(i);

						Entity* entity_type = prefabs->at(i);
						sf::Sprite entitySprite = entity_type->get_sprite();

						// TODO ADK find texture rect for these type entities only once
						sf::IntRect display_rect;
						sf::Vector2i textureBounds(entitySprite.getTexture()->getSize());
						int fsx = entity_type->sprite_sheet.frame_size.x;
						int fsy = entity_type->sprite_sheet.frame_size.y;
						int numFramesWide = textureBounds.x / ((fsx > 0) ? fsx : 1);
						int numFramesTall = textureBounds.y / ((fsy > 0) ? fsy : 1);
						int xLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame % (numFramesWide > 0 ? numFramesWide : 1)) * fsx;
						int yLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame / (numFramesWide > 0 ? numFramesWide : 1)) * fsy;
						display_rect.left = xLoc;
						display_rect.top = yLoc;
						display_rect.width = entitySprite.getTextureRect().width;
						display_rect.height = entitySprite.getTextureRect().height;
						entitySprite.setTextureRect(display_rect);

						if (ImGui::ImageButton(entitySprite, sf::Vector2f(40.f, 40.f)))
						{
							entity_selected_for_creation = prefabs->at(i);
							b_prefab_for_creation = true;
							curr_tool = TOOL_PLACE;
						}
						ImGui::Text(entity_type->prefab_id);
						ImGui::PopID();

						++i;
						if (i < prefabs->size())
						{
							ImGui::PushID(i);

							entity_type = prefabs->at(i);
							entitySprite = entity_type->get_sprite();

							display_rect;
							sf::Vector2i textureBounds(entitySprite.getTexture()->getSize());
							fsx = entity_type->sprite_sheet.frame_size.x;
							fsy = entity_type->sprite_sheet.frame_size.y;
							numFramesWide = textureBounds.x / ((fsx > 0) ? fsx : 1);
							numFramesTall = textureBounds.y / ((fsy > 0) ? fsy : 1);
							xLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame % (numFramesWide > 0 ? numFramesWide : 1)) * fsx;
							yLoc = (entity_type->sprite_sheet.animations[entity_type->sprite_sheet.selected_animation].start_frame / (numFramesWide > 0 ? numFramesWide : 1)) * fsy;
							display_rect.left = xLoc;
							display_rect.top = yLoc;
							display_rect.width = entitySprite.getTextureRect().width;
							display_rect.height = entitySprite.getTextureRect().height;
							entitySprite.setTextureRect(display_rect);

							if (ImGui::ImageButton(entitySprite, sf::Vector2f(40.f, 40.f)))
							{
								entity_selected_for_creation = prefabs->at(i);
								b_prefab_for_creation = true;
								curr_tool = TOOL_PLACE;
							}
							ImGui::Text(entity_type->prefab_id);
							ImGui::PopID();
						}
					}
					ImGui::EndGroup();
					ImGui::SameLine();
				}

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();

		ImGui::EndTabItem();
	}
	ImGui::EndTabBar();

	ImGui::End();
}

void Scene_Editor::draw_menu_and_optionsbar_ui()
{
	ImGui::Begin("Menu and  Editor Options", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(0.f, 0.f));
	ImGui::SetWindowSize(sf::Vector2f((float) active_editor_config.window_size_x, (float) active_editor_config.top_left_pixel.y));

	ImGui::PushItemWidth(120.f);
	ImGui::InputText("LEVEL ID", level_id, 30);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Level ID is the name of the level.");
		ImGui::TextUnformatted("Save path in /Assets/Levels/.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	if (ImGui::IsItemActive())
	{
		b_typing_in_textbox = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		load_level_confirmation(level_id);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		save_level_confirmation(level_id);
	}
	ImGui::SameLine();

	if (ImGui::Button("Config"))
	{
		b_show_config = !b_show_config;
	}
	if (b_show_config)
	{
		ImGui::Begin("ADK Editor Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(sf::Vector2f((float)active_editor_config.top_left_pixel.x, (float)active_editor_config.top_left_pixel.y));
		ImGui::SetWindowSize(sf::Vector2f((float)active_editor_config.bot_right_pixel.x - active_editor_config.top_left_pixel.x,
			(float)active_editor_config.bot_right_pixel.y - active_editor_config.top_left_pixel.y));

		// Big grid
		ImGui::Text("Big Grid: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(90.f);
		ImGui::InputInt("Big Grid X", &active_editor_config.big_grid_x);
		ImGui::SameLine();
		ImGui::InputInt("Big Grid Y", &active_editor_config.big_grid_y);
		ImGui::SameLine();
		ImGui::Checkbox("Show Big Grid", &active_editor_config.b_show_big_grid);
		ImGui::SameLine();
		ImGui::PushItemWidth(130.f);
		ImColor im_col_temp = MoreColors::sfcolor_to_imcolor(active_editor_config.big_grid_color);
		if (ImGui::ColorEdit4("Big Grid Color", (float*)&im_col_temp, ImGuiColorEditFlags_NoInputs))
		{
			entity_selected_for_creation = nullptr;
		}
		active_editor_config.big_grid_color = MoreColors::imcolor_to_sfcolor(im_col_temp);

		// Background color
		

		// Copy-Paste delay


		// Misc
		ImGui::Checkbox("Debug render mode", &b_debug_render);
		ImGui::SameLine();
		// Collider Debug Color
		im_col_temp = MoreColors::sfcolor_to_imcolor(active_editor_config.collider_debug_color);
		if (ImGui::ColorEdit4("Collider Debug Color", (float*)&im_col_temp, ImGuiColorEditFlags_NoInputs))
		{
			entity_selected_for_creation = nullptr;
			level_entities.debug_color = active_editor_config.collider_debug_color;
		}
		active_editor_config.collider_debug_color = MoreColors::imcolor_to_sfcolor(im_col_temp);

		im_col_temp = MoreColors::sfcolor_to_imcolor(active_editor_config.editor_background_color);
		if (ImGui::ColorEdit4("Editor Background Color", (float*)&im_col_temp, ImGuiColorEditFlags_NoInputs))
		{
			entity_selected_for_creation = nullptr;
		}
		active_editor_config.editor_background_color = MoreColors::imcolor_to_sfcolor(im_col_temp);

		// View
		ImGui::InputFloat("Default View Resolution X", &view_config.size_x);
		ImGui::InputFloat("Default View Resolution Y", &view_config.size_y);
		ImGui::InputFloat("Default View Position X", &view_config.position_x);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
			ImGui::TextUnformatted("The x position of the CENTER of the view.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
		ImGui::InputFloat("Default View Position Y", &view_config.position_y);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
			ImGui::TextUnformatted("The y position of the CENTER of the view.");
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		// Save Config
		if (ImGui::Button("Save Editor Configs"))
		{
			// Load config
			std::ifstream config_load("Saved/Config/Editor.json");
			nlohmann::json config_json;
			config_load >> config_json;
			// Change config
			config_json["GridSizeX"] = active_editor_config.grid_size_x;
			config_json["GridSizeY"] = active_editor_config.grid_size_y;
			config_json["bShowGrid"] = active_editor_config.b_show_grid;
			config_json["bSnapGrid"] = active_editor_config.b_snap_to_grid;
			config_json["GridColorR"] = active_editor_config.grid_color.r;
			config_json["GridColorG"] = active_editor_config.grid_color.g;
			config_json["GridColorB"] = active_editor_config.grid_color.b;
			config_json["GridColorA"] = active_editor_config.grid_color.a;
			config_json["SelectColorR"] = active_editor_config.selection_color.r;
			config_json["SelectColorG"] = active_editor_config.selection_color.g;
			config_json["SelectColorB"] = active_editor_config.selection_color.b;
			config_json["SelectColorA"] = active_editor_config.selection_color.a;
			config_json["DepthFilterLower"] = active_editor_config.depth_filter_lowerbound;
			config_json["DepthFilterUpper"] = active_editor_config.depth_filter_upperbound;
			config_json["BigGridSizeX"] = active_editor_config.big_grid_x;
			config_json["BigGridSizeY"] = active_editor_config.big_grid_y;
			config_json["bShowBigGrid"] = active_editor_config.b_show_big_grid;
			config_json["BigGridColorR"] = active_editor_config.big_grid_color.r;
			config_json["BigGridColorG"] = active_editor_config.big_grid_color.g;
			config_json["BigGridColorB"] = active_editor_config.big_grid_color.b;
			config_json["BigGridColorA"] = active_editor_config.big_grid_color.a;
			config_json["ColliderDebugColorR"] = active_editor_config.collider_debug_color.r;
			config_json["ColliderDebugColorG"] = active_editor_config.collider_debug_color.g;
			config_json["ColliderDebugColorB"] = active_editor_config.collider_debug_color.b;
			config_json["ColliderDebugColorA"] = active_editor_config.collider_debug_color.a;			
			config_json["EditorBackgroundColorR"] = active_editor_config.editor_background_color.r;
			config_json["EditorBackgroundColorG"] = active_editor_config.editor_background_color.g;
			config_json["EditorBackgroundColorB"] = active_editor_config.editor_background_color.b;
			config_json["EditorBackgroundColorA"] = active_editor_config.editor_background_color.a;
			config_json["bDebugRenderMode"] = b_debug_render;
			config_json["DefaultViewResolutionX"] = view_config.size_x;
			config_json["DefaultViewResolutionY"] = view_config.size_y;
			config_json["DefaultViewPositionX"] = view_config.position_x;
			config_json["DefaultViewPositionY"] = view_config.position_y;
			// Save config
			std::ofstream config_stream_engine;
			config_stream_engine.open("Saved/Config/Editor.json");
			config_stream_engine << config_json.dump(4);
			config_stream_engine.close();
			// Make active_editor_config the default for this session
			default_editor_config = active_editor_config;
		}

		if (ImGui::Button("Reset Editor Configs"))
		{
			// Load config
			std::ifstream config_load("Saved/Config/Editor.json");
			nlohmann::json config_json;
			config_load >> config_json;
			// Change config
			FEditorConfig default_config;
			FViewConfig default_view;
			config_json["GridSizeX"] = default_config.grid_size_x;
			config_json["GridSizeY"] = default_config.grid_size_y;
			config_json["bShowGrid"] = default_config.b_show_grid;
			config_json["bSnapGrid"] = default_config.b_snap_to_grid;
			config_json["GridColorR"] = default_config.grid_color.r;
			config_json["GridColorG"] = default_config.grid_color.g;
			config_json["GridColorB"] = default_config.grid_color.b;
			config_json["GridColorA"] = default_config.grid_color.a;
			config_json["SelectColorR"] = default_config.selection_color.r;
			config_json["SelectColorG"] = default_config.selection_color.g;
			config_json["SelectColorB"] = default_config.selection_color.b;
			config_json["SelectColorA"] = default_config.selection_color.a;
			config_json["DepthFilterLower"] = default_config.depth_filter_lowerbound;
			config_json["DepthFilterUpper"] = default_config.depth_filter_upperbound;
			config_json["BigGridSizeX"] = default_config.big_grid_x;
			config_json["BigGridSizeY"] = default_config.big_grid_y;
			config_json["bShowBigGrid"] = default_config.b_show_big_grid;
			config_json["BigGridColorR"] = default_config.big_grid_color.r;
			config_json["BigGridColorG"] = default_config.big_grid_color.g;
			config_json["BigGridColorB"] = default_config.big_grid_color.b;
			config_json["BigGridColorA"] = default_config.big_grid_color.a;
			config_json["ColliderDebugColorR"] = default_config.collider_debug_color.r;
			config_json["ColliderDebugColorG"] = default_config.collider_debug_color.g;
			config_json["ColliderDebugColorB"] = default_config.collider_debug_color.b;
			config_json["ColliderDebugColorA"] = default_config.collider_debug_color.a;
			config_json["EditorBackgroundColorR"] = default_config.editor_background_color.r;
			config_json["EditorBackgroundColorG"] = default_config.editor_background_color.g;
			config_json["EditorBackgroundColorB"] = default_config.editor_background_color.b;
			config_json["EditorBackgroundColorA"] = default_config.editor_background_color.a;
			config_json["bDebugRenderMode"] = false;
			view_config.size_x = config_json["DefaultViewResolutionX"];
			view_config.size_y = config_json["DefaultViewResolutionY"];
			view_config.position_x = config_json["DefaultViewPositionX"];
			view_config.position_y = config_json["DefaultViewPositionY"];
			// Save config
			std::ofstream config_stream_engine;
			config_stream_engine.open("Saved/Config/Editor.json");
			config_stream_engine << config_json.dump(4);
			config_stream_engine.close();
			// Make default_editor_config the active one
			active_editor_config = default_editor_config;
		}

		ImGui::End();
	}

	ImGui::SameLine(370.f);

	ImGui::PushItemWidth(90.f);
	ImGui::InputInt("Grid Size X", &active_editor_config.grid_size_x);
	ImGui::SameLine();
	ImGui::InputInt("Grid Size Y", &active_editor_config.grid_size_y);
	ImGui::SameLine();
	ImGui::Checkbox("Show Grid", &active_editor_config.b_show_grid);
	ImGui::SameLine();
	ImGui::Checkbox("Snap to Grid", &active_editor_config.b_snap_to_grid);
	ImGui::SameLine();

	ImGui::PushItemWidth(130.f);
	ImColor gridCol = MoreColors::sfcolor_to_imcolor(active_editor_config.grid_color);
	if (ImGui::ColorEdit4("Grid Color", (float*)&gridCol, ImGuiColorEditFlags_NoInputs))
	{
		entity_selected_for_creation = nullptr;
	}	
	if (ImGui::IsItemActive())
	{
		entity_selected_for_creation = nullptr;
	}
	active_editor_config.grid_color = MoreColors::imcolor_to_sfcolor(gridCol);
	ImGui::SameLine();

	ImColor selCol = MoreColors::sfcolor_to_imcolor(active_editor_config.selection_color);
	if (ImGui::ColorEdit4("Select Color", (float*)&selCol, ImGuiColorEditFlags_NoInputs))
	{
		entity_selected_for_creation = nullptr;
	}
	if (ImGui::IsItemActive())
	{
		entity_selected_for_creation = nullptr;
	}
	active_editor_config.selection_color = MoreColors::imcolor_to_sfcolor(selCol);
	ImGui::SameLine(1180.f);

	ImGui::Text("Show Depth From");
	ImGui::SameLine();
	ImGui::PushItemWidth(90.f);
	ImGui::InputInt("To", &active_editor_config.depth_filter_lowerbound);
	ImGui::SameLine();
	ImGui::InputInt("###", &active_editor_config.depth_filter_upperbound);
	ImGui::SameLine();

	// RESET VIEW button
	if (ImGui::Button("Reset View"))
	{
		initialize_scene_view(*render_window_ptr);
	}	
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Reset View (A)");
		ImGui::TextUnformatted("Reset the view back to the original view.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::End();
}

void Scene_Editor::draw_tools_menu_ui()
{
	ImGui::Begin("Tools Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(sf::Vector2f(0.f, (float) active_editor_config.top_left_pixel.y));
	ImGui::SetWindowSize(sf::Vector2f((float)active_editor_config.top_left_pixel.x, 
		(float)active_editor_config.window_size_y - active_editor_config.top_left_pixel.y - (active_editor_config.window_size_y - active_editor_config.bot_right_pixel.y)));

	ImGui::Text("TOOLS");
	// Selection tool
	if (ImGui::ImageButton(select_button, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, curr_tool == TOOL_SELECTION ? sf::Color::Black : sf::Color::White))
	{
		curr_tool = TOOL_SELECTION;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Selection Tool (F)");
		ImGui::TextUnformatted("Select an entity from the level viewport. Drag them around to reposition them.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Place tool
	if (ImGui::ImageButton(place_button, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, curr_tool == TOOL_PLACE ? sf::Color::Black : sf::Color::White))
	{
		curr_tool = TOOL_PLACE;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Place Tool (D)");
		ImGui::TextUnformatted("Select an entity type from the Entity Types window and place/create entities inside the level.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Brush tool 
	if (ImGui::ImageButton(brush_button, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, curr_tool == TOOL_BRUSH ? sf::Color::Black : sf::Color::White))
	{
		curr_tool = TOOL_BRUSH;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Brush Tool (S)");
		ImGui::TextUnformatted("Select an entity type from the Entity Types window and place/create MULTIPLE entities at a time. Probably a good idea to turn on 'Snap to Grid' for this.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	// Picker tool
	if (ImGui::ImageButton(picker_button, sf::Vector2f(30.f, 30.f), -1, sf::Color::Transparent, curr_tool == TOOL_PICKER ? sf::Color::Black : sf::Color::White))
	{
		curr_tool = TOOL_PICKER;
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.f);
		ImGui::TextUnformatted("Picker Tool (A)");
		ImGui::TextUnformatted("Select an entity type by picking from the level viewport.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

	ImGui::End();
}

void Scene_Editor::initialize_scene_view(sf::RenderWindow& window)
{
	view_config.zoom = 1.f;
	Scene::initialize_scene_view(window);
	sf::View curr_view = window.getView();
	curr_view.setViewport(sf::FloatRect(0.03f, 0.04f, 0.78f, 0.738f));
	window.setView(curr_view);
	zoom_factor = view_config.zoom;
}

void Scene_Editor::set_entity_selected_for_properties(Entity* newSelection)
{
	// Set new entity
	ImGui::DeactivateActiveItem();
	entity_selected_for_properties = newSelection;
}

void Scene_Editor::update_editor_config_with_window(sf::RenderWindow& window)
{
	// Change default editor config window size
	default_editor_config.window_size_x = window.getSize().x;
	default_editor_config.window_size_y = window.getSize().y;
	default_editor_config.top_left_pixel.x = (int)(0.03f * window.getSize().x);
	default_editor_config.top_left_pixel.y = (int)(0.04f * window.getSize().y);
	default_editor_config.bot_right_pixel.x = (int)(0.78f * window.getSize().x + default_editor_config.top_left_pixel.x);
	default_editor_config.bot_right_pixel.y = (int)(0.738f * window.getSize().y + default_editor_config.top_left_pixel.y);
	// Change active editor config window size
	active_editor_config.window_size_x = default_editor_config.window_size_x;
	active_editor_config.window_size_y = default_editor_config.window_size_y;
	active_editor_config.top_left_pixel = default_editor_config.top_left_pixel;
	active_editor_config.bot_right_pixel = default_editor_config.bot_right_pixel;
	// Change editor bg rect size
	bg_rect = sf::FloatRect((float)active_editor_config.top_left_pixel.x, (float)active_editor_config.top_left_pixel.y,
		(float)active_editor_config.bot_right_pixel.x - active_editor_config.top_left_pixel.x, (float)active_editor_config.bot_right_pixel.y - active_editor_config.top_left_pixel.y);

	// resize the level viewer as well
	sf::View curr_view = window.getView();
	curr_view.setSize(bg_rect.width, bg_rect.height);
	curr_view.zoom(zoom_factor);
	window.setView(curr_view);
}

void Scene_Editor::brush_place_helper()
{
	// Get the current mouse position in the window
	sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
	// Convert it to world coordinates
	sf::Vector2f worldPos = (*render_window_ptr).mapPixelToCoords(pixelPos);
	// Calculate amount to subtract if snapping to grid
	int sX = 0;
	int sY = 0;
	if (active_editor_config.b_snap_to_grid)
	{
		sX = (int)worldPos.x % active_editor_config.grid_size_x;
		if (worldPos.x < 0)
		{
			sX = active_editor_config.grid_size_x + sX;
		}
		sY = (int)worldPos.y % active_editor_config.grid_size_y;
		if (worldPos.y < 0)
		{
			sY = active_editor_config.grid_size_y + sY;
		}
	}

	// Get would-be position of a new entity
	int posX = ((int)worldPos.x) - sX;
	int posY = ((int)worldPos.y) - sY;

	// Check if position already visited (only for brush tool)
	if (curr_tool == TOOL_BRUSH)
	{
		for (auto pos : brush_visited_positions)
		{
			if (pos.x == posX && pos.y == posY)
			{
				// If we've visited this position, then don't place a new entity
				return;
			}
		}
		// Visit this position
		brush_visited_positions.push_back(sf::Vector2i(posX, posY));
	}
	
	// Create a new entity
	Entity* created = entity_selected_for_creation->class_description_ptr->constructor();
	if (b_prefab_for_creation)
	{
		// Copy prefab entity if we are placing a prefab object
		Entity::copy(*created, *entity_selected_for_creation);
	}
	else
	{
		// Just load default texture if we are placing a base object
		created->load_default_texture();
	}
	created->set_position((float)posX, (float)posY);

	// Add the entity to this scene/level editor's entity list
	level_entities.add(created);
	level_entities.mark_depth_changed();
	// Set this entity to be selected
	set_entity_selected_for_properties(created);

	//std::cout << "Placed " << created->EntityId << " at x: " << worldPos.x << "  y: " << worldPos.y << std::endl;
}