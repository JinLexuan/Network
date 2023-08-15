set(NP_TARGET Client)

set(NP_SOURCES
    SyncClient.cpp
    main.cpp)

add_executable(${NP_TARGET} ${NP_SOURCES})

target_include_directories(${NP_TARGET} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${NP_TARGET} PRIVATE
     ${NP_LIBRARIES})
     
set_target_properties(${NP_TARGET} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build")