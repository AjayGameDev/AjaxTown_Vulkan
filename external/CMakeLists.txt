cmake_minimum_required(VERSION 3.31)

project(External)

include(FetchContent)
set(FetchContent_QUIET OFF)

# Global settings
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# Ensure all dependencies use the same MSVC Runtime (/MT)
#set(USE_STATIC_CRT ON CACHE BOOL "" FORCE)

# Fetch spdlog
set(SPDLOG_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
        GIT_TAG "v1.15.3"
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)


FetchContent_Declare(
        VMA
        GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
        GIT_TAG "v3.3.0"
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(VMA)


# Fetch SDL

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(
        sdl
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-3.4.0"
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sdl)

# Fetch assimp

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE) # disable all exporters to debloat
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE) # disable all importers to debloat
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)

set(ASSIMP_INSTALL ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "" FORCE)
set(USE_STATIC_CRT OFF CACHE BOOL "" FORCE) # ON = /MT static , OFF = /MD dynamic
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY "https://github.com/assimp/assimp"
        GIT_TAG "v6.0.2"
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(assimp)

# IMPORTANT: If linking statically, Assimp needs this definition to
# remove __declspec(dllimport) from headers.
if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DASSIMP_DLL) # Some versions use this logic
    # Better: target-based approach
    target_compile_definitions(assimp INTERFACE ASSIMP_BUILD_STATIC_LIB)
endif()

install( TARGETS

        spdlog
        VulkanMemoryAllocator

        EXPORT External
)

install(EXPORT External
        FILE ExternalConfig.cmake
        NAMESPACE External::
        DESTINATION lib/cmake/External
)


