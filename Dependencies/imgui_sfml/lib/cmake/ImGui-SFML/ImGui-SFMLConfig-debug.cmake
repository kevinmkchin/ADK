#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ImGui-SFML::ImGui-SFML" for configuration "Debug"
set_property(TARGET ImGui-SFML::ImGui-SFML APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ImGui-SFML::ImGui-SFML PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/ImGui-SFML.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ImGui-SFML::ImGui-SFML )
list(APPEND _IMPORT_CHECK_FILES_FOR_ImGui-SFML::ImGui-SFML "${_IMPORT_PREFIX}/lib/ImGui-SFML.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
