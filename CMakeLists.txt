cmake_minimum_required(VERSION 3.14)
project(SH-T LANGUAGES CXX)

# === Compiler Settings ===
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Project Structure ===
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/include)
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/example)

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/main.cpp)

include_directories(
    ${INC_DIR}
    ${INC_DIR}/core
    ${INC_DIR}/ecs
    ${INC_DIR}/ecs/core
    ${INC_DIR}/ecs/systems
    ${INC_DIR}/ecs/components
    ${INC_DIR}/ecs/events
    ${INC_DIR}/utils
    ${INC_DIR}/scripts
    ${INC_DIR}/editor
    ${INC_DIR}/world
    ${INC_DIR}/serializers
    ${INC_DIR}/external
    ${INC_DIR}/external/imgui
    ${INC_DIR}/external/imgui/backends
    ${INC_DIR}/external/sol
)

add_executable(main ${SRC_FILES})
target_compile_definitions(main PRIVATE USE_STD_FILESYSTEM)

# === Dependency Handling ===
if(WIN32)
    # vcpkg-style packages
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(lua CONFIG REQUIRED)

    target_link_libraries(main
        SDL2::SDL2main
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        SDL2_mixer::SDL2_mixer
        lua::lua
    )

elseif(UNIX)
    # Native Linux packages
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    find_package(Lua REQUIRED)

    include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIR}
    )

    target_link_libraries(main
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${LUA_LIBRARIES}
    )
endif()

# === Post-build asset copying ===
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:main>/assets
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:main>/textures
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:main>/sounds
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:main>/scripts
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:main>/scenes
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} $<TARGET_FILE_DIR:main>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/imgui.ini $<TARGET_FILE_DIR:main>
    COMMENT "[INFO] Copied assets and created runtime folders."
)

# === Optional run target ===
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:main> ./main
    DEPENDS main
    COMMENT "[INFO] Launching binary from build dir..."
)
