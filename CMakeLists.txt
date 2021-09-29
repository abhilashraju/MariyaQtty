
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(Server VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS ON)

if(MSVC)
  set (Boost_USE_STATIC_RUNTIME ON)
  add_definitions(-D_WIN32_WINNT=0x0601)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
  add_definitions(-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
  add_compile_options(/MP)
  string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)
SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/Users/rabhil/work/boost_1_69_0")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Users/rabhil/work/boost_1_69_0/lib")
# find_package(Boost REQUIRED COMPONENTS system)
SET(Boost_INCLUDE_DIRS "C:/Users/rabhil/work/boost_1_69_0")
SET(Boost_LIBRARY_DIRS "C:/Users/rabhil/work/boost_1_69_0/lib" "C:/Users/rabhil/work/boost_1_69_0/stage/lib")
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

file(GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})
add_subdirectory(src/core)


add_executable(${PROJECT_NAME} examples/server.cpp )
target_link_libraries(${PROJECT_NAME} PUBLIC mariaqtty)
target_include_directories(${PROJECT_NAME} PUBLIC src/core)

if(NOT WIN32)
  find_package(Threads)
  target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT ${PROJECT_NAME})