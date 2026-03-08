cmake_minimum_required(VERSION 3.15)
project(computer-graphics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
  SOURCE_DIRECTORY
  ${CMAKE_SOURCE_DIR}
)

set(
  INCLUDE_DIRECTORY
  ${CMAKE_SOURCE_DIR}/modules
)

file(
  GLOB_RECURSE
  SOURCE_FILES
  ${SOURCE_DIRECTORY}/*.cpp
)

include(FetchContent)
cmake_policy(SET CMP0135 NEW)

FetchContent_Declare(
  json
  URL
  https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(json)

add_executable(
  ${PROJECT_NAME}
  ${SOURCE_FILES}
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${INCLUDE_DIRECTORY})

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  nlohmann_json::nlohmann_json
)