macro(configure_environment launch_executable main_executable)

    set(CMAKE_CXX_STANDARD 20)

    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -Wl,--allow-multiple-definition")
    set(CMAKE_SHARED_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static  -Wl,--allow-multiple-definition")

    add_subdirectory(external/fmt)

    add_subdirectory(lib)

    add_subdirectory(template)
    add_subdirectory(loader)

    add_custom_target(package DEPENDS loader template cheat)

    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/package/")
    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/package/cheats/")

    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:loader> "${CMAKE_CURRENT_BINARY_DIR}/package/")
    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:libcheat> "${CMAKE_CURRENT_BINARY_DIR}/package/")
    add_custom_command(TARGET package POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:cheat> "${CMAKE_CURRENT_BINARY_DIR}/package/cheats/")

    target_compile_definitions(loader PRIVATE LIBCHEAT_LAUNCH_EXECUTABLE_PATH=L"${launch_executable}")
    target_compile_definitions(loader PRIVATE LIBCHEAT_PROCESS_NAME=L"${main_executable}")

endmacro()