# cmake/Coverage.cmake
# ──────────────────────────────────────────────────────────────────────────────
# Adds gcov/lcov coverage instrumentation to a test executable and creates
# a 'coverage' target that runs the tests and produces an HTML report.
#
# Usage:
#   include(cmake/Coverage.cmake)
#   calculator_add_coverage(
#       TEST_TARGET   calculator_tests        # executable to instrument
#       INCLUDE_DIRS  "${PROJECT_SOURCE_DIR}/include"  # paths to keep in report
#       OUTPUT_DIR    "${CMAKE_BINARY_DIR}/coverage_html"
#   )
# ──────────────────────────────────────────────────────────────────────────────
function(calculator_add_coverage)
    cmake_parse_arguments(
        ARG                     # prefix
        ""                      # options
        "TEST_TARGET;OUTPUT_DIR"
        "INCLUDE_DIRS"          # multi-value
        ${ARGN}
    )

    # ── Require GCC/Clang ─────────────────────────────────────────────────────
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(WARNING
            "[Coverage] Coverage requires GCC or Clang. "
            "Current compiler: ${CMAKE_CXX_COMPILER_ID}. Skipping."
        )
        return()
    endif()

    # ── Locate tools ──────────────────────────────────────────────────────────
    find_program(LCOV_EXECUTABLE    lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(NOT LCOV_EXECUTABLE)
        message(WARNING
            "[Coverage] lcov not found — 'coverage' target will not be created. "
            "Install lcov to enable HTML coverage reports."
        )
        return()
    endif()

    if(NOT GENHTML_EXECUTABLE)
        message(WARNING
            "[Coverage] genhtml not found — HTML report generation disabled."
        )
        return()
    endif()

    message(STATUS "[Coverage] lcov   found: ${LCOV_EXECUTABLE}")
    message(STATUS "[Coverage] genhtml found: ${GENHTML_EXECUTABLE}")

    # ── Instrument the test target ────────────────────────────────────────────
    # --coverage is equivalent to -fprofile-arcs -ftest-coverage (compile)
    # and -lgcov (link). Must use Debug / -O0 for accurate line mapping.
    target_compile_options(${ARG_TEST_TARGET} PRIVATE
        --coverage
        -O0
        -g
        -fno-inline
    )
    target_link_options(${ARG_TEST_TARGET} PRIVATE --coverage)

    # ── Build lcov --extract filters from INCLUDE_DIRS ────────────────────────
    # Each directory becomes a glob pattern passed to lcov --extract.
    set(_extract_patterns "")
    foreach(_dir ${ARG_INCLUDE_DIRS})
        list(APPEND _extract_patterns "${_dir}/*")
    endforeach()

    # ── Coverage target ───────────────────────────────────────────────────────
    set(_raw_info     "${CMAKE_BINARY_DIR}/coverage_raw.info")
    set(_filtered_info "${CMAKE_BINARY_DIR}/coverage_filtered.info")
    set(_output_dir   "${ARG_OUTPUT_DIR}")

    add_custom_target(coverage
        # 0. Reset counters from previous runs
        COMMAND ${LCOV_EXECUTABLE}
            --zerocounters
            --directory "${CMAKE_BINARY_DIR}"

        # 1. Run tests
        COMMAND "${CMAKE_CTEST_COMMAND}"
            --test-dir "${CMAKE_BINARY_DIR}"
            --output-on-failure

        # 2. Capture all .gcda files into a raw info file
        COMMAND ${LCOV_EXECUTABLE}
            --capture
            --directory "${CMAKE_BINARY_DIR}"
            --output-file "${_raw_info}"
            --ignore-errors mismatch,inconsistent,inconsistent
            --rc geninfo_unexecuted_blocks=1

        # 3. Keep only project headers — strip GTest, stdlib, etc.
        COMMAND ${LCOV_EXECUTABLE}
            --extract "${_raw_info}"
            ${_extract_patterns}
            --output-file "${_filtered_info}"

        # 4. Generate HTML report
        COMMAND ${GENHTML_EXECUTABLE}
            "${_filtered_info}"
            --output-directory "${_output_dir}"
            --title "${PROJECT_NAME} Coverage Report"
            --legend
            --show-details

        # 5. Print the result path
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan
            "Coverage report: ${_output_dir}/index.html"

        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "Running tests and generating lcov HTML coverage report..."
        VERBATIM
    )

    set_target_properties(coverage PROPERTIES FOLDER "Coverage")

    message(STATUS
        "[Coverage] Target 'coverage' created. "
        "Run: cmake --build <build-dir> --target coverage"
    )
endfunction()
