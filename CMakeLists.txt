cmake_minimum_required(VERSION 2.8)

PROJECT(Multiplayer_Minesweeper C CXX)

set(CMAKE_CXX_STANDARD 17)

# set(TGUI_STATIC_LIBRARIES TRUE)

set(TGUI_DIR ${CMAKE_SOURCE_DIR}/lib/TGUI-0.8.7/build)
FIND_PACKAGE(TGUI 0.8.7 REQUIRED)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/TGUI-0.8.7/include)
LINK_DIRECTORIES(${TGUI_DIR}/lib)


# set(SFML_STATIC_LIBRARIES TRUE)

set(SFML_DIR ${CMAKE_SOURCE_DIR}/lib/SFML-2.5.1/build)
FIND_PACKAGE(SFML 2.5 COMPONENTS network graphics window audio system REQUIRED)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib/SFML-2.5.1/include)
LINK_DIRECTORIES(${SFML_DIR}/lib)

#override cmake internal compiler settings
SET(CMAKE_C_COMPILER "/usr/bin/clang" CACHE STRING "clang compiler" FORCE)

#add the include folder, so that the compiler will find all files there
INCLUDE_DIRECTORIES(include)

file(GLOB SOURCE_FILES src/*.cpp src/*/*.cpp)
file(GLOB HEADER_FILES include/*.hpp include/*/*.hpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

ADD_EXECUTABLE(Minesweeper
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

TARGET_LINK_LIBRARIES(Minesweeper tgui sfml-network sfml-graphics sfml-window sfml-audio sfml-system)

#Youcompleteme
SET( CMAKE_EXPORT_COMPILE_COMMANDS 1 )
