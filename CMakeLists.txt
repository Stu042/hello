## To build and run from command line use:
# cmake -S . -B build
# cmake --build build

## Exe will be in build/Debug
# cd build
# ./hello

## To create the project, I made the src, libs and headers folders.
# cd libs
# git clone https://github.com/libsdl-org/SDL.git
# cd SDL ; git switch SDL2 ; cd ..
# git clone https://github.com/libsdl-org/SDL_image.git
# cd SDL_image ; git switch SDL2 ; cd ..
## glew is awkward, go to https://github.com/nigels-com/glew/releases
## download glew-2.2.0-win32.zip and save it it in libs/glew-win-2.2.0
## for other OSes download glew-2.2.0.tgz and save it it in libs/glew-2.2.0
## To update SDL2 or SDL2_image,
## cd to their directory and pull latest from github, i.e.
# cd libs/SDL_Image
# git pull


cmake_minimum_required(VERSION 3.31)
project(hello)

# I keep .c files in src and .h files in headers
include_directories(headers)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Add SDL2 repo
add_subdirectory(libs/SDL EXCLUDE_FROM_ALL)

# Add SDL2_image repo
add_subdirectory(libs/SDL_image EXCLUDE_FROM_ALL)

# opengl is part of your graphics drivers or kernel, so just need to ask for it
find_package(OpenGL REQUIRED)

set(SRC_FILES src/main.c)

# Add glew
if (WIN32)
    include_directories(libs/glew-win-2.2.0/include)
    add_library(glew STATIC IMPORTED GLOBAL)
    set_target_properties(glew PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-win-2.2.0/bin/Release/x64/glew32.dll)
    set_target_properties(glew PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-win-2.2.0/include)
    set_target_properties(glew PROPERTIES LINKER_LANGUAGE C)

    # Create your game executable target as usual
    add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

    # Link to the actual SDL2 library.
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image glew ${OPENGL_LIBRARIES})

else ()
    include_directories(libs/glew-2.2.0/include)
    set(GLEW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-2.2.0/src/glew.c)
    set(SRC_FILES ${SRC_FILES} ${GLEW_SRC})

    # Create your game executable target as usual
    add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

    # Link to the actual SDL2 library.
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image ${OPENGL_LIBRARIES})
endif ()




# copy assets
add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

# add copied assets to clean list
set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

# copy dll manually
add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glew-win-2.2.0/bin/Release/x64/glew32.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glew32.dll)

# add copied dll to clean list
set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glew32.dll)
