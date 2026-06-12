cmake_minimum_required(VERSION 3.15)
project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
  SOURCE_DIRECTORY
  ${CMAKE_SOURCE_DIR}
)

set(
  INCLUDE_DIRECTORY
  ${CMAKE_SOURCE_DIR}
)

file(
  GLOB_RECURSE
  SOURCE_FILES
  ${SOURCE_DIRECTORY}/*.cpp
)

include(FetchContent)
cmake_policy(SET CMP0135 NEW)

FetchContent_Declare(
  nlohmann_json
  URL
  https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_Declare(
  OpenEXR
  URL
  https://github.com/AcademySoftwareFoundation/openexr/releases/download/v3.4.9/openexr-3.4.9.tar.gz
)

FetchContent_Declare(
  stb
  GIT_REPOSITORY
  https://github.com/nothings/stb.git
)

FetchContent_MakeAvailable(nlohmann_json)
FetchContent_MakeAvailable(OpenEXR)
FetchContent_MakeAvailable(stb)

add_library(stb_image INTERFACE)

add_executable(
  ${PROJECT_NAME}
  ${SOURCE_FILES}
)

target_include_directories(
  stb_image
  INTERFACE
  ${stb_SOURCE_DIR}
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${INCLUDE_DIRECTORY})

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  $<$<CONFIG:Debug>:-g;-fsanitize=address,undefined>
  $<$<CONFIG:Release>:-O3;-march=native;-mtune=native;-flto>
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  nlohmann_json::nlohmann_json
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  OpenEXR::OpenEXR
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  stb_image
)