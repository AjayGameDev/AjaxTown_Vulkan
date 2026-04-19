#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "External::spdlog" for configuration "Release"
set_property(TARGET External::spdlog APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(External::spdlog PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/spdlog.lib"
  )

list(APPEND _cmake_import_check_targets External::spdlog )
list(APPEND _cmake_import_check_files_for_External::spdlog "${_IMPORT_PREFIX}/lib/spdlog.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
