#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "MoreColors.h"
#include "ADKMath.h"
#include "Scene_Editor.h"
#include "ADKSaveLoad.h"
#include "ADKAssets.h"
#include "../ADKEditorMetaRegistry.h"

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
	, default_copy_paste_timer(0.5f)
	, copy_paste_timer(default_copy_paste_timer)
	, b_show_config(false)
	, b_debug_render(false)
	, b_collision_match_sprite_bound(false)
{
}

void Scene_Editor::begin_scene(sf::RenderWindow& window)
{
	LOG("Began Editor Scene");

	// TODO load editor config from ini

	// Setup big grid
	default_editor_config.big_grid_x = (int) view_config.size_x;
	default_editor_config.big_grid_y = (int) view_config.size_y;
	// Setup window values
	update_editor_config_with_window(window);

	// Load button textures
	select_button.loadFromFile("Assets/adk/button_selection.png");
	place_button.loadFromFile("Assets/adk/button_place.png");
	brush_button.loadFromFile("Assets/adk/button_brush.png");
	picker_button.loadFromFile("Assets/adk/button_picker.png");

	// Initialize ImGui::SFML process
	ImGui::SFML::Init(window);
	render_window_ptr = &window;

#pragma region ImGuiStyle
	ImGuiStyle * style = &ImGui::GetStyle();
	//style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	//style->FramePadding = ImVec2(5, 5);
	//style->FrameRounding = 4.0f;
	//style->ItemSpacing = ImVec2(12, 8);
	//style->ItemInnerSpacing = ImVec2(8, 6);
	//style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 17.0f;
	style->ScrollbarRounding = 9.0f;
	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4((float)23 / (float)255, (float)29 / (float)255, (float)34 / (float)255, 1.00f);//ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.467f, 0.533f, 0.6f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.467f, 0.6f, 0.6f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.251f, 0.267f, 1.00f);
#pragma endregion

	// Setup view
	initialize_scene_view(window);

	// Make entity of every type to display to editor
	for (auto iter = ADKEditorMetaRegistry::Identifiers.begin(); iter != ADKEditorMetaRegistry::Identifiers.end(); ++iter)
	{
		Entity* created = ADKEditorMetaRegistry::CreateNewEntity(*iter);
		created->load_default_texture();
		created->entity_id = *iter;
		entity_types.add(created);
		// At this point, Identifiers[0] represents the id of the entity type of entity at EntityTypes.at(0)
	}

	texture_dialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	texture_dialog.SetTitle("Choose a texture to use");
	texture_dialog.SetTypeFilters({ ".png", ".jpg" });

	//window.setKeyRepeatEnabled(false);
}

void Scene_Editor::end_scene(sf::RenderWindow& window)
{
	// Shut down ImGui::SFML process
	ImGui::SFML::Shutdown();

	// Deallocate entity memory
	for (int i = entity_types.size() - 1; i != -1; --i)
	{
		entity_types.remove_and_destroy(entity_types.at(i));
	}
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
	if (sf::Event::KeyPressed)
	{
		if (b_typing_level_id == false)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			{
				curr_tool = TOOL_SELECTION;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				curr_tool = TOOL_PLACE;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				curr_tool = TOOL_BRUSH;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				curr_tool = TOOL_PICKER;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				active_editor_config.b_show_grid = !active_editor_config.b_show_grid;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
			{
				active_editor_config.b_snap_to_grid = !active_editor_config.b_snap_to_grid;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
		{
			if (entity_selected_for_properties != nullptr)
			{
				entities.remove_and_destroy(entity_selected_for_properties);
				set_entity_selected_for_properties(nullptr);
			}
		}
	}

	// Editor features that should only happen when sprite sheet viewer is closed.
	if (b_texture_show == false && b_show_config == false && texture_dialog.IsOpened() == false)
	{
		// Select entity
		if (curr_tool == TOOL_SELECTION && event.mouseButton.button == sf::Mouse::Left && event.type == sf::Event::MouseButtonPressed)
		{
			sf::IntRect viewWindow(active_editor_config.top_left_pixel.x, active_editor_config.top_left_pixel.y,
				active_editor_config.bot_right_pixel.x - active_editor_config.top_left_pixel.x, active_editor_config.bot_right_pixel.y - active_editor_config.top_left_pixel.y);
			for (int i = entities.size() - 1; i > -1; --i)
			{
				Entity* at = entities.at(i);
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
				scene_view.zoom(event.mouseWheel.delta > 0 ? 0.9f : 1.1f);
				render_window_ptr->setView(scene_view);
			}
		}
	
		// Ent rotate with Left Alt
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
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
		else if (sf::Event::KeyReleased && event.key.code == sf::Keyboard::LAlt)
		{
			b_alt_rotate = false;
		}

		// Ent scale with Left Ctrl
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
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
		else if (sf::Event::KeyReleased && event.key.code == sf::Keyboard::LShift)
		{
			b_shift_scale = false;
		}

		// Copy
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			if (entity_selected_for_properties != nullptr)
			{
				Entity::copy(copied_entity, *entity_selected_for_properties);
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
			Entity* created = ADKEditorMetaRegistry::CreateNewEntity(copied_entity.entity_id);
			Entity::copy(*created, copied_entity);
			created->set_position((float)posX, (float)posY);
			created->init_collider();
			// Add the entity to this scene/level editor's entity list
			entities.add(created);
			entities.mark_depth_changed();
			// Set this entity to be selected
			set_entity_selected_for_properties(created);

			copy_paste_timer = default_copy_paste_timer;
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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
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
		scene_view.move(sf::Vector2f(xM, yM));
		render_window_ptr->setView(scene_view);
	}

	// Entity move with Arrows
	if (entity_selected_for_properties != nullptr && b_typing_level_id == false)
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
	entities.update(deltaTime);

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
		scene_view.move(-delta * zoom_factor);
		render_window_ptr->setView(scene_view);
		last_mouse_pos = pixelPos;
	}

	// Alt Rotate
	if (b_alt_rotate && entity_selected_for_properties != nullptr)
	{
		sf::Vector2i pixelPos = sf::Mouse::getPosition(*render_window_ptr);
		sf::Vector2f mousePos = (*render_window_ptr).mapPixelToCoords(pixelPos);
		sf::Vector2f vec2 = mousePos - entity_selected_for_properties->get_position();
		float angle = ADKMath::get_angle_between_vectors(vec1, vec2);
		entity_selected_for_properties->set_rotation(og_rot + angle, b_collision_match_sprite_bound);
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

		entity_selected_for_properties->set_scale(og_scale * scale, b_collision_match_sprite_bound);
	}

	// Decrement copy paste timer
	copy_paste_timer -= deltaTime;

	// ImGui::SFML Update
	ImGui::SFML::Update(*render_window_ptr, sf::seconds(deltaTime));
	// Setup ImGui to draw
	draw_editor_ui();
}

void Scene_Editor::update_post(float deltaTime)
{

}

void Scene_Editor::render_pre(sf::RenderWindow& window)
{

}

void Scene_Editor::render(sf::RenderWindow& window)
{
	entities.render_with_depth(window, active_editor_config.depth_filter_lowerbound, active_editor_config.depth_filter_upperbound);
	if (b_debug_render)
	{
		entities.render_with_depth(window, active_editor_config.depth_filter_lowerbound, active_editor_config.depth_filter_upperbound, true);
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
	// Render ImGui::SFML. All creation of ImGui widgets must happen between ImGui::SFML::Update and ImGui::SFML::Render.
	ImGui::SFML::Render(window);
}

void Scene_Editor::draw_editor_ui()
{
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
			ImGui::InputInt("Y Position", &y);
			entity_selected_for_properties->set_position((float) x, (float) y);

			float angle = entity_selected_for_properties->get_rotation();
			ImGui::InputFloat("Rotation", &angle);
			entity_selected_for_properties->set_rotation(angle, b_collision_match_sprite_bound);

			float scale = entity_selected_for_properties->get_scale();
			ImGui::InputFloat("Scale", &scale);
			entity_selected_for_properties->set_scale(scale, b_collision_match_sprite_bound);

			int depth = entity_selected_for_properties->get_depth();
			ImGui::InputInt("Depth", &depth);
			if (entity_selected_for_properties->get_depth() != depth)
			{
				entities.mark_depth_changed();
			}
			entity_selected_for_properties->set_depth(depth);

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
			entity_selected_for_properties->set_visible(v);

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
			entity_selected_for_properties->set_active(a);

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region SpriteSheetProperties
			ImGui::Separator();
			ImGui::Text("SpriteSheet Properties");
			ImGui::Separator();

			// Choose Texture
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
						entity_selected_for_properties->match_framesize_to_texture();
					}
				}
				texture_dialog.ClearSelected();
			}

			int si[2];
			si[0] = entity_selected_for_properties->sprite_sheet.frame_size.x;
			si[1] = entity_selected_for_properties->sprite_sheet.frame_size.y;

			ImGui::InputInt2("Frame Size", si);
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

			ImGui::Dummy(ImVec2(0, 7));
#pragma endregion

#pragma region CollisionProperties
			ImGui::Separator();
			ImGui::Text("Collision Properties");
			ImGui::Separator();

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
#pragma endregion
		}

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Entities in Level"))
	{
		// Two columns
		ImGui::Columns(2);

		// Display all entities currently in level
		int selected = -1;
		std::vector<std::string> bruh;
		for (int i = 0; i < entities.size(); ++i)
		{
			std::string name = std::to_string(i).append(" ");
			name = entities.at(i)->entity_id.empty() ? name.append(typeid(entities.at(i)).name()) : name.append(entities.at(i)->entity_id);
			bruh.push_back(name);
			if (entities.at(i) == entity_selected_for_properties)
			{
				selected = i;
			}
		}
		if (ImGui::ListBox("", &selected, bruh))
		{
			set_entity_selected_for_properties(entities.at(selected));
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
		std::string num = std::to_string(entities.size()).append(" entities in level.");
		const char * numInCharPtr = num.c_str();
		ImGui::Text(numInCharPtr);

		// Delete Button
		if (ImGui::Button("Delete Selected"))
		{
			if (entity_selected_for_properties != nullptr)
			{
				entities.remove_and_destroy(entity_selected_for_properties);
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

	// Display all available entity types
	for (int i = 0; i < entity_types.size(); ++i)
	{
		ImGui::BeginGroup();
		{
			sf::Sprite entitySprite = entity_types.at(i)->get_sprite();
			const char* EntityId = ADKEditorMetaRegistry::Identifiers[i].c_str();
			if (ImGui::ImageButton(entitySprite, sf::Vector2f(30.f, 30.f)))
			{
				entity_selected_for_creation = entity_types.at(i);
				curr_tool = TOOL_PLACE;
			}
			ImGui::Text(EntityId);

			//++i;
			//if (i < EntityTypes.size())
			//{
			//	entitySprite = EntityTypes.at(i)->GetSprite();
			//	EntityId = ADKEditorMetaRegistry::Identifiers[i].c_str();
			//	if (ImGui::ImageButton(entitySprite, sf::Vector2f(50.f, 50.f)))
			//	{
			//		EntitySelectedForCreation = EntityTypes.at(i);
			//		currTool = TOOL_PLACE;
			//	}
			//	ImGui::Text(EntityId);
			//}
		}
		ImGui::EndGroup();
		ImGui::SameLine();
	}

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
		b_typing_level_id = true;
	}
	else
	{
		b_typing_level_id = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		ADKSaveLoad Saver;
		Saver.save_scene(level_id, *this);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		ADKSaveLoad Loader;
		Loader.load_to_scene(level_id, *this);
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
		ImColor bigGridCol = MoreColors::sfcolor_to_imcolor(active_editor_config.big_grid_color);
		if (ImGui::ColorEdit4("Big Grid Color", (float*)&bigGridCol, ImGuiColorEditFlags_NoInputs))
		{
			entity_selected_for_creation = nullptr;
		}
		active_editor_config.big_grid_color = MoreColors::imcolor_to_sfcolor(bigGridCol);

		// Background color
		

		// Copy-Paste delay


		// Misc
		ImGui::Checkbox("Debug render mode", &b_debug_render);

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
	active_editor_config.grid_color = MoreColors::imcolor_to_sfcolor(gridCol);
	ImGui::SameLine();

	ImColor selCol = MoreColors::sfcolor_to_imcolor(active_editor_config.selection_color);
	if (ImGui::ColorEdit4("Select Color", (float*)&selCol, ImGuiColorEditFlags_NoInputs))
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
	scene_view.setViewport(sf::FloatRect(0.03f, 0.04f, 0.78f, 0.738f));
	Scene::initialize_scene_view(window);
	zoom_factor = view_config.zoom;
}

void Scene_Editor::set_entity_selected_for_properties(Entity* newSelection)
{
	// Clear the modified sprite color of the last entity
	if (entity_selected_for_properties != nullptr)
	{
		entity_selected_for_properties->get_sprite().setColor(sf::Color::White);
	}
	// Set new entity
	entity_selected_for_properties = newSelection;
	// Modify sprite color of new entity
	if (entity_selected_for_properties != nullptr)
	{
		entity_selected_for_properties->get_sprite().setColor(sf::Color::White); // set to whatever modified color u want
	}
}

void Scene_Editor::update_editor_config_with_window(sf::RenderWindow& window)
{
	default_editor_config.window_size_x = window.getSize().x;
	default_editor_config.window_size_y = window.getSize().y;
	default_editor_config.top_left_pixel.x = (int)(0.03f * window.getSize().x);
	default_editor_config.top_left_pixel.y = (int)(0.04f * window.getSize().y);
	default_editor_config.bot_right_pixel.x = (int)(0.78f * window.getSize().x + default_editor_config.top_left_pixel.x);
	default_editor_config.bot_right_pixel.y = (int)(0.738f * window.getSize().y + default_editor_config.top_left_pixel.y);
	active_editor_config = default_editor_config;
	bg_rect = sf::FloatRect((float)active_editor_config.top_left_pixel.x, (float)active_editor_config.top_left_pixel.y,
		(float)active_editor_config.bot_right_pixel.x - active_editor_config.top_left_pixel.x, (float)active_editor_config.bot_right_pixel.y - active_editor_config.top_left_pixel.y);

	// resize the level viewer as well
	scene_view.setSize(bg_rect.width, bg_rect.height);
	scene_view.zoom(zoom_factor);
	window.setView(scene_view);
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
	Entity* created = ADKEditorMetaRegistry::CreateNewEntity(entity_selected_for_creation->entity_id);
	created->load_default_texture();
	// Assign it the specific entity id for its entity type
	created->entity_id = entity_selected_for_creation->entity_id;
	created->set_position((float)posX, (float)posY);
	// Initialize collider position
	created->init_collider();
	// Add the entity to this scene/level editor's entity list
	entities.add(created);
	entities.mark_depth_changed();
	// Set this entity to be selected
	set_entity_selected_for_properties(created);

	//std::cout << "Placed " << created->EntityId << " at x: " << worldPos.x << "  y: " << worldPos.y << std::endl;
}