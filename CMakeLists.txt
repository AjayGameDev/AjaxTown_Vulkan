    cmake_minimum_required(VERSION 3.31)

    project(AjaxTown)

    set(CMAKE_CXX_STANDARD 20)



    if(ANDROID)
        set(External_Source "${CMAKE_CURRENT_LIST_DIR}/build/deps_android_arm64/install")
    else()
        set(External_Source "${CMAKE_CURRENT_LIST_DIR}/build/deps_windows/install")
    endif ()

    list(APPEND CMAKE_PREFIX_PATH "${External_Source}") #Search packages here


    # Gather source files
    file(GLOB_RECURSE AjaxTownSourceFiles CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"
    )

    # Create the main target first
    if(ANDROID)
        add_library(main SHARED ${AjaxTownSourceFiles})
        # The Android toolchain file sets CMAKE_FIND_ROOT_PATH_MODE_PACKAGE to ONLY by default
        # which tells CMake to only look inside the Android NDK's internal folders
        # This allows CMake to search both the Android sysroot and your custom installation directory
        set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
        set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
        set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    else()
        add_executable(main ${AjaxTownSourceFiles})
    endif()

    if(DEFINED TARGET_PLATFORM)
        target_compile_definitions(main PRIVATE TARGET_PLATFORM_${TARGET_PLATFORM})
        message(STATUS "Target platform : ${TARGET_PLATFORM}")
    else()
        message(FATAL_ERROR "Add target platform in cmake profile! ")
    endif()

    if(DEFINED TARGET_PLATFORM_MODE)
        target_compile_definitions(main PRIVATE TARGET_PLATFORM_MODE_${TARGET_PLATFORM_MODE})
        message(STATUS "Target platform mode: ${TARGET_PLATFORM_MODE}")
    else ()
        message(FATAL_ERROR "Add target platform mode in cmake profile! ")
    endif ()

    find_package(SDL3 CONFIG REQUIRED)
    find_package(Threads REQUIRED)
    find_package(assimp REQUIRED CONFIG)
    find_package(External CONFIG REQUIRED)

    
    #target_compile_definitions(main PRIVATE ASSIMP_DLL)
    # Note: Newer Assimp versions (v6+) handle this better,
    # but keep this in mind if you get "undefined reference" errors.

    target_link_libraries(main PRIVATE
            SDL3::SDL3-static
            assimp::assimp
            External::spdlog
            External::VulkanMemoryAllocator
            External::ktx
    )
    # Ensure no ASSIMP_DLL is present anywhere
    target_compile_definitions(main PRIVATE ASSIMP_STATIC_LIB)

    target_precompile_headers(main PRIVATE source/pch.h)

    # Include directories
    target_include_directories(main PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/source")




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
        target_link_libraries(main PRIVATE ${VULKAN_LIB} log android)

    endif()
