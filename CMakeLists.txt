    cmake_minimum_required(VERSION 3.31)

    project(AjaxTown)

    set(CMAKE_CXX_STANDARD 20)

    include(FetchContent)

    # Fetch SDL
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
            sdl
            GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
            GIT_TAG "release-3.2.16"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(sdl)

    #Fetch spdlog
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
            GIT_TAG "v1.15.3"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)

    #Fetch VMA
    FetchContent_Declare(
            VMA
            GIT_REPOSITORY "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git"
            GIT_TAG "v3.3.0"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(VMA)

    #Fetch SPIRV-Cross
    FetchContent_Declare(
            SPIRVCross
            GIT_REPOSITORY "https://github.com/KhronosGroup/SPIRV-Cross"
            GIT_TAG "vulkan-sdk-1.4.328.1"
            GIT_SHALLOW TRUE
    )
    FETCHCONTENT_MAKEAVAILABLE(SPIRVCross)

    # Gather source files
    file(GLOB_RECURSE AjaxTownSourceFiles CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"
    )

    # Create the main target first
    if(ANDROID)
        add_library(main SHARED ${AjaxTownSourceFiles})
    else()
        add_executable(main ${AjaxTownSourceFiles})
    endif()

    target_precompile_headers(main PRIVATE source/pch.h)

    # Include directories
    target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source")

    # Include VMA headers
    target_include_directories(main PRIVATE ${vma_SOURCE_DIR}/include)

    # Link SDL
    target_link_libraries(main PRIVATE SDL3::SDL3)

    # Link VMA
    target_link_libraries(main PRIVATE GPUOpen::VulkanMemoryAllocator)

    # Link spdlog in debug mode
    target_link_libraries(main PRIVATE spdlog::spdlog)

    # Link SPIRV Cross
    target_link_libraries(main PRIVATE spirv-cross-core spirv-cross-glsl)


    # Vulkan setup
    if(WIN32)
        find_package(Vulkan REQUIRED)
        include_directories($ENV{VULKAN_SDK}/include)
        target_link_libraries(main PRIVATE Vulkan::Vulkan)

    elseif(ANDROID)
        find_library(VULKAN_LIB vulkan)
        if(NOT VULKAN_LIB)
            message(FATAL_ERROR "Vulkan library not found")
        endif()
        target_link_libraries(main PRIVATE ${VULKAN_LIB})
    endif()