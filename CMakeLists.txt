cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(ADK)


file(GLOB sources . src/*.cpp)
file(GLOB engine_sources . src/Engine/*.cpp)

# add WIN32 after ${PROJECT_NAME} if compile for SUBSYSTEM:WINDOWS
add_executable(${PROJECT_NAME} ${sources} ${engine_sources})

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