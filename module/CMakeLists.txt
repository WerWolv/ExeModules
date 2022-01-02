project(module)

add_library(${PROJECT_NAME} SHARED
        source/main.cpp

        source/interface.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".mod")
target_link_libraries(${PROJECT_NAME} PRIVATE libmodule)

target_compile_definitions(${PROJECT_NAME} PRIVATE LOGGER_SOURCE_NAME="${PROJECT_NAME}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/package/modules/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_BINARY_DIR}/package/modules/")