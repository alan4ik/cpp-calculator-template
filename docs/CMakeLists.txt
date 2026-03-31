# docs/CMakeLists.txt
# ──────────────────────────────────────────────────────────────────────────────
# Pull in the helper function and create the 'docs' target.
# ──────────────────────────────────────────────────────────────────────────────
include(${CMAKE_SOURCE_DIR}/cmake/Doxygen.cmake)

calculator_add_doxygen(
    TARGET          docs
    INPUT_DIRS      "${CMAKE_SOURCE_DIR}/include"
    OUTPUT_DIR      "${CMAKE_CURRENT_BINARY_DIR}"
    DOXYFILE_IN     "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
    DOXYFILE_OUT    "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    PROJECT_NAME    "${PROJECT_NAME}"
    PROJECT_VERSION "${PROJECT_VERSION}"
)
