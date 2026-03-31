cmake_minimum_required(VERSION 3.20)

# ──────────────────────────────────────────────────────────────────────────────
# Project declaration
# ──────────────────────────────────────────────────────────────────────────────
project(
    Calculator
    VERSION      1.0.0
    DESCRIPTION  "Header-only C++11 arithmetic calculator library"
    HOMEPAGE_URL "https://github.com/example/calculator"
    LANGUAGES    CXX
)

# ──────────────────────────────────────────────────────────────────────────────
# Global C++ settings  (do NOT set on INTERFACE targets — set per-target)
# ──────────────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)   # -std=c++11, not -std=gnu++11

# Keep build-tree .cmake files from leaking into install trees
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ──────────────────────────────────────────────────────────────────────────────
# Guard against in-source builds
# ──────────────────────────────────────────────────────────────────────────────
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Please create a separate build directory:\n"
        "  cmake -S . -B build"
    )
endif()

# ──────────────────────────────────────────────────────────────────────────────
# Only expose options when this is the top-level project
# ──────────────────────────────────────────────────────────────────────────────
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(CALCULATOR_BUILD_TESTS  "Build unit tests"       ON)
    option(CALCULATOR_BUILD_DOCS   "Build Doxygen docs"     ON)
    option(CALCULATOR_INSTALL      "Generate install rules" ON)

    # Nice IDE folder layout
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# ──────────────────────────────────────────────────────────────────────────────
# INTERFACE library  (header-only — no compiled artefacts)
# ──────────────────────────────────────────────────────────────────────────────
add_library(calculator INTERFACE)

# Canonical namespaced alias so consumers can write:
#   target_link_libraries(my_app PRIVATE Calculator::calculator)
add_library(Calculator::calculator ALIAS calculator)

target_include_directories(calculator
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>           # relative to CMAKE_INSTALL_PREFIX
)

target_compile_features(calculator INTERFACE cxx_std_11)

# Optional: enable stricter warnings for consumers who opt in
target_compile_options(calculator
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ──────────────────────────────────────────────────────────────────────────────
# Sub-directories
# ──────────────────────────────────────────────────────────────────────────────
if(CALCULATOR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CALCULATOR_BUILD_DOCS)
    add_subdirectory(docs)
endif()

# ──────────────────────────────────────────────────────────────────────────────
# Installation & packaging
# ──────────────────────────────────────────────────────────────────────────────
if(CALCULATOR_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Headers
    install(
        DIRECTORY   include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Targets file
    install(
        TARGETS     calculator
        EXPORT      CalculatorTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        EXPORT      CalculatorTargets
        FILE        CalculatorTargets.cmake
        NAMESPACE   Calculator::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Calculator
    )

    # Config file (find_package support)
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CalculatorConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/CalculatorConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Calculator
    )

    # Version file
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/CalculatorConfigVersion.cmake
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/CalculatorConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/CalculatorConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Calculator
    )
endif()
