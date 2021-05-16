cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(ADK)

# add WIN32 after ${PROJECT_NAME} if compile for SUBSYSTEM:WINDOWS
add_executable(${PROJECT_NAME}
		src/Engine/ADKCamera.cpp
		src/Engine/ADKGameStatics.cpp
		src/Engine/ADKMenu.cpp
		src/Engine/ADKPrefabFactory.cpp
		src/Engine/ADKRaycast.cpp
		src/Engine/ADKReflection.cpp
		src/Engine/ADKTimer.cpp
		src/Engine/ADKSaveLoad.cpp
		src/Engine/ADKTextures.cpp
		src/Engine/ADKAssets.cpp
		src/Engine/BoxCollider.cpp
		src/Engine/Engine.cpp
		src/Engine/Entity.cpp
		src/Engine/EntityList.cpp
		src/Engine/imgui-SFML.cpp
		src/Engine/imgui.cpp
		src/Engine/imgui_demo.cpp
		src/Engine/imgui_draw.cpp
		src/Engine/imgui_widgets.cpp
		src/Engine/Scene.cpp
		src/Engine/Scene_Editor.cpp
		src/Engine/TileEntity.cpp

		src/PlatformerPackage/GrapplehookPlayer.cpp
		src/PlatformerPackage/PlatformerDamageBox.cpp
		src/PlatformerPackage/PlatformerMovingDamageBox.cpp
		src/PlatformerPackage/PlatformerOneWayTile.cpp
		src/PlatformerPackage/PlatformerSpawn.cpp
		src/PlatformerPackage/PlatformerTile.cpp
		src/PlatformerPackage/PlatformerActivePlatform.cpp
		src/PlatformerPackage/PlatformerFallingPlatform.cpp
		src/PlatformerPackage/PlatformerMovingPlatform.cpp
		src/PlatformerPackage/PlatformerPlayer.cpp
		src/PlatformerPackage/PlatformerSpikes.cpp
		src/PlatformerPackage/PlatformerTrampoline.cpp
		src/PlatformerPackage/PlatformerTriggerBox.cpp
		src/PlatformerPackage/Scene_PlatformerGame.cpp
		)

#add_compile_definitions()

# Additional Include Directories
include_directories(${PROJECT_NAME}
    PUBLIC lib/imgui_sfml/include
    PUBLIC lib/SFML/include
)

# Additional Library/Dependency Directories
target_link_directories(${PROJECT_NAME} 
    PRIVATE lib/SFML/lib
)

# Additional Dependencies
target_link_libraries(${PROJECT_NAME}
	sfml-system-d.lib
	sfml-graphics-d.lib
	sfml-audio-d.lib
	sfml-window-d.lib
	sfml-network-d.lib
	sfml-main-d.lib
	opengl32.lib
)