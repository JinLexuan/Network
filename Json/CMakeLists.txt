set(NP_TARGET json)

set(NP_SOURCES
    main.cpp)

find_package(nlohmann_json REQUIRED)

add_executable(${NP_TARGET} ${NP_SOURCES})

target_link_libraries(${NP_TARGET} PRIVATE 
    nlohmann_json::nlohmann_json)
     
set_target_properties(${NP_TARGET} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")