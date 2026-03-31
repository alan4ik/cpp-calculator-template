# tests/CMakeLists.txt
# ──────────────────────────────────────────────────────────────────────────────
# GoogleTest — prefer system installation, fall back to FetchContent
# ──────────────────────────────────────────────────────────────────────────────
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(STATUS "[Tests] System GTest not found — fetching via FetchContent")
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
        GIT_SHALLOW    TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST           OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
else()
    message(STATUS "[Tests] Using system GTest: ${GTest_VERSION}")
endif()

# ──────────────────────────────────────────────────────────────────────────────
# Test executable
# ──────────────────────────────────────────────────────────────────────────────
add_executable(calculator_tests
    test_calculator.cpp
)

target_link_libraries(calculator_tests
    PRIVATE
        Calculator::calculator  # the header-only library under test
        GTest::gtest_main       # includes main() + GTest + GMock
)

target_compile_features(calculator_tests PRIVATE cxx_std_11)

# Treat warnings as errors only for our own code, not for GTest
target_compile_options(calculator_tests
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ──────────────────────────────────────────────────────────────────────────────
# Register with CTest
# ──────────────────────────────────────────────────────────────────────────────
include(GoogleTest)
gtest_discover_tests(
    calculator_tests
    PROPERTIES
        TIMEOUT 30
)

set_target_properties(calculator_tests PROPERTIES FOLDER "Tests")
