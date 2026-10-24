cmake_minimum_required(VERSION 3.20)
project(Pathfinder C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw  GIT_REPOSITORY https://github.com/glfw/glfw.git    GIT_TAG 3.4     GIT_SHALLOW TRUE)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG v1.91.5 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(glfw imgui)
find_package(OpenGL REQUIRED)

if(MINGW)
    add_link_options(-static)
endif()

# Graphics-independent logic; the tests link against this too.
add_library(pathfinder_core STATIC
    src/core/Grid.cpp
    src/core/MovementRules.cpp
    src/core/Heuristic.cpp
    src/core/Reachability.cpp
    src/algorithms/SearchState.cpp
    src/algorithms/BestFirstPathfinder.cpp
    src/algorithms/BfsPathfinder.cpp
    src/algorithms/RandomWalkPathfinder.cpp
    src/algorithms/QuantumWalkPathfinder.cpp
    src/algorithms/PathfinderFactory.cpp
    src/generation/MudSprinkler.cpp
    src/generation/RandomMapGenerator.cpp
    src/generation/MazeGenerator.cpp
    src/repository/TextMapRepository.cpp
    src/service/Agent.cpp
    src/service/SimulationService.cpp
    src/config/IniConfigLoader.cpp)
target_include_directories(pathfinder_core PUBLIC src)
target_compile_options(pathfinder_core PRIVATE -Wall -Wextra)

add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp)
target_include_directories(imgui_lib PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

add_executable(pathfinder WIN32
    src/main.cpp
    src/audio/WinSoundPlayer.cpp
    src/config/ExecutableDirectory.cpp
    src/ui/GridLayout.cpp
    src/ui/GridRenderer.cpp
    src/ui/InputController.cpp
    src/ui/ControlPanel.cpp
    src/ui/Application.cpp)
target_compile_options(pathfinder PRIVATE -Wall -Wextra)
target_link_libraries(pathfinder PRIVATE pathfinder_core imgui_lib winmm)

# Copy the config file and sounds next to the exe.
add_custom_command(TARGET pathfinder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:pathfinder>/config
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/sfx $<TARGET_FILE_DIR:pathfinder>/sfx)

enable_testing()
add_executable(pathfinder_tests tests/tests.cpp)
target_link_libraries(pathfinder_tests PRIVATE pathfinder_core)
add_test(NAME pathfinder_tests COMMAND pathfinder_tests)
