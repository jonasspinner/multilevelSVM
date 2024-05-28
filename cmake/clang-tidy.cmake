find_program(CLANG_TIDY clang-tidy)

if(NOT CLANG_TIDY)
    message(STATUS "Clang-Tidy was not found, check and fix targets are inaccessible")
else()
    add_custom_target(clang-tidy-check COMMAND ${CLANG_TIDY} ${ALL_SOURCES} -p=${CMAKE_CURRENT_BINARY_DIR} -use-color
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
        COMMENT "Checking code with Clang-Tidy"
    )

    add_custom_target(clang-tidy-fix COMMAND ${CLANG_TIDY} ${ALL_SOURCES} -p=${CMAKE_CURRENT_BINARY_DIR} -use-color -fix-errors -format-style=file
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
        COMMENT "Checking code and applying automatic fixes with Clang-Tidy"
    )
    
    message(STATUS "Clang-Tidy was found, 'clang-tidy-check' and 'clang-tidy-fix' targets can be used")
endif()

find_program(RUN_CLANG_TIDY run-clang-tidy)

if(NOT RUN_CLANG_TIDY)
    message(STATUS "Run-Clang-Tidy was not found, check and fix (parallel) targets are inaccessible")
else()
    add_custom_target(run-clang-tidy-check COMMAND ${RUN_CLANG_TIDY} -p=${CMAKE_CURRENT_BINARY_DIR} # -use-color
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
        COMMENT "Checking code with run-clang-tidy (parallel)"
    )

    add_custom_target(run-clang-tidy-fix COMMAND ${RUN_CLANG_TIDY} -p=${CMAKE_CURRENT_BINARY_DIR} -format -fix # -use-color
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
        COMMENT "Checking code and applying automatic fixes with run-clang-tidy (parallel)"
    )

    message(STATUS "Run-Clang-Tidy was found, 'run-clang-tidy-check' and 'run-clang-tidy-fix' (parallel) targets can be used")
endif()
