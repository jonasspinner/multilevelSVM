find_program(CPPCHECK cppcheck)

if(NOT CPPCHECK)
    message(STATUS "Cppcheck was not found, check target is inaccessible")
else()
    cmake_host_system_information(RESULT N_PROC QUERY NUMBER_OF_LOGICAL_CORES)

    set(CPPCHECK_BUILD_DIR ${CMAKE_BINARY_DIR}/cppcheck-build-dir)
    file(MAKE_DIRECTORY ${CPPCHECK_BUILD_DIR})

    add_custom_target(cppcheck-check COMMAND ${CPPCHECK} 
        # --check-level=exhaustive                                           # Cppcheck >=2.11
        # --checkers-report=${CMAKE_BINARY_DIR}/cppcheck-checks-report.log   # Cppcheck >=2.12 - run with clean build dir, Cppcheck stores data there, so results can be different in subsequent runs
        --cppcheck-build-dir=${CPPCHECK_BUILD_DIR}
        --enable=all
        --error-exitcode=1
        --inconclusive                                                      # False positive can happen
        --language=c++
        --platform=native
        --project=${CMAKE_BINARY_DIR}/compile_commands.json
        # --suppress=checkersReport                                         # Cppcheck >=2.13
        -i ${CMAKE_BINARY_DIR}
        -j ${N_PROC}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
        COMMENT "Checking code with Cppcheck"
    )
    
    message(STATUS "Cppcheck was found, 'cppcheck-check' target can be used")
endif()
