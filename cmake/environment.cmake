macro(configure_environment launch_executable main_executable)

    set(CMAKE_CXX_STANDARD 20)

    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -Wl,--allow-multiple-definition")
    set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static  -Wl,--allow-multiple-definition")

    add_subdirectory(external/fmt)

    add_subdirectory(lib)

    add_subdirectory(module)
    add_subdirectory(loader)

    add_custom_target(package DEPENDS loader module libmodule)

    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/package/")

    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:loader> "${CMAKE_BINARY_DIR}/package/")
    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libmodule> "${CMAKE_BINARY_DIR}/package/")

    target_compile_definitions(loader PRIVATE LAUNCH_EXECUTABLE_PATH=L"${launch_executable}")
    target_compile_definitions(loader PRIVATE MAIN_PROCESS_NAME=L"${main_executable}")

endmacro()