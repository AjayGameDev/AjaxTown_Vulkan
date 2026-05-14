#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "External::spdlog" for configuration "Release"
set_property(TARGET External::spdlog APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(External::spdlog PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libspdlog.a"
  )

list(APPEND _cmake_import_check_targets External::spdlog )
list(APPEND _cmake_import_check_files_for_External::spdlog "${_IMPORT_PREFIX}/lib/libspdlog.a" )

# Import target "External::ktx" for configuration "Release"
set_property(TARGET External::ktx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(External::ktx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libktx.a"
  )

list(APPEND _cmake_import_check_targets External::ktx )
list(APPEND _cmake_import_check_files_for_External::ktx "${_IMPORT_PREFIX}/lib/libktx.a" )

# Import target "External::astcenc-neon-static" for configuration "Release"
set_property(TARGET External::astcenc-neon-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(External::astcenc-neon-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libastcenc-neon-static.a"
  )

list(APPEND _cmake_import_check_targets External::astcenc-neon-static )
list(APPEND _cmake_import_check_files_for_External::astcenc-neon-static "${_IMPORT_PREFIX}/lib/libastcenc-neon-static.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
