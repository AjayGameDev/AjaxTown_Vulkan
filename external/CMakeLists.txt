cmake_minimum_required(VERSION 3.31)

project(External)

include(FetchContent)
set(FetchContent_QUIET OFF)

# Fetch spdlog
set(SPDLOG_INSTALL ON CACHE BOOL "" FORCE)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
        GIT_TAG "v1.15.3"
        GIT_SHALLOW TRUEcd
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


