cmake_minimum_required(VERSION 3.5)

project(NP)

find_package(Boost REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -Wno-pragmas -Wno-comment")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(NP_TARGET Network)
# set(NP_SOURCES
#     main.cpp
# )

find_package(Boost REQUIRED)
set(NP_LIBRARIES
    Boost::boost
)

# include(Archive/CMakeLists.txt)

# add_executable(${NP_TARGET} ${NP_SOURCES})

# target_include_directories(${NP_TARGET} PRIVATE 
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${Boost_INCLUDE_DIRS}
# )

# target_link_libraries(${NP_TARGET} ${NP_LIBRARIES})

add_subdirectory("SyncClient")
add_subdirectory("SyncServer")