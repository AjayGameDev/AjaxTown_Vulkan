#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KTX::ktx" for configuration "Release"
set_property(TARGET KTX::ktx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KTX::ktx PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libktx.a"
  )

list(APPEND _cmake_import_check_targets KTX::ktx )
list(APPEND _cmake_import_check_files_for_KTX::ktx "${_IMPORT_PREFIX}/lib/libktx.a" )

# Import target "KTX::astcenc-neon-static" for configuration "Release"
set_property(TARGET KTX::astcenc-neon-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KTX::astcenc-neon-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libastcenc-neon-static.a"
  )

list(APPEND _cmake_import_check_targets KTX::astcenc-neon-static )
list(APPEND _cmake_import_check_files_for_KTX::astcenc-neon-static "${_IMPORT_PREFIX}/lib/libastcenc-neon-static.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
