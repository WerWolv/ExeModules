project(libmodule)

add_library(${PROJECT_NAME} SHARED
        source/helpers/path.cpp
        source/helpers/inject_file.cpp
        source/helpers/process.cpp
        source/helpers/utils.cpp

        source/api/loader.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt-header-only)

target_compile_definitions(${PROJECT_NAME} PRIVATE LOGGER_SOURCE_NAME="lib" LIBRARY_FILE_NAME="$<TARGET_FILE_NAME:${PROJECT_NAME}>")