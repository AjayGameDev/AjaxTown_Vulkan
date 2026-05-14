#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "External::spdlog" for configuration "MinSizeRel"
set_property(TARGET External::spdlog APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(External::spdlog PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/spdlog.lib"
  )

list(APPEND _cmake_import_check_targets External::spdlog )
list(APPEND _cmake_import_check_files_for_External::spdlog "${_IMPORT_PREFIX}/lib/spdlog.lib" )

# Import target "External::ktx" for configuration "MinSizeRel"
set_property(TARGET External::ktx APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(External::ktx PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/ktx.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/ktx.dll"
  )

list(APPEND _cmake_import_check_targets External::ktx )
list(APPEND _cmake_import_check_files_for_External::ktx "${_IMPORT_PREFIX}/lib/ktx.lib" "${_IMPORT_PREFIX}/bin/ktx.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
