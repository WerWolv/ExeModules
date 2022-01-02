project(loader)

add_executable(loader
        source/main.cpp
)

target_link_libraries(loader PRIVATE libmodule)
target_compile_definitions(loader PRIVATE LOGGER_SOURCE_NAME="Loader")