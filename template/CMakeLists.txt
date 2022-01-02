project(cheat)

add_library(${PROJECT_NAME} SHARED
        source/main.cpp

        source/interface.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".cht")
target_link_libraries(${PROJECT_NAME} PRIVATE libcheat)

target_compile_definitions(cheat PRIVATE LOGGER_SOURCE_NAME="${PROJECT_NAME}")