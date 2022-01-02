project(libcheat)

add_library(libcheat SHARED
        source/helpers/path.cpp
        source/helpers/inject_file.cpp
        source/helpers/process.cpp
        source/helpers/utils.cpp

        source/api/loader.cpp
)

set_target_properties(libcheat PROPERTIES PREFIX "")

target_include_directories(libcheat PUBLIC include)
target_link_libraries(libcheat PUBLIC fmt::fmt-header-only)

target_compile_definitions(libcheat PRIVATE LOGGER_SOURCE_NAME="lib" LIBCHEAT_FILE_NAME="$<TARGET_FILE_NAME:libcheat>")