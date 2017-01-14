cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 11)

find_tml_package(tml)
find_tml_package(minecraft)
include_directories(${TML_INCLUDE_DIRS} ${MINECRAFT_INCLUDE_DIRS})

add_library(assetloader SHARED src/main.cpp)
target_link_libraries(assetloader ${TML_LIBRARIES} ${MINECRAFT_LIBRARIES})
