# cmake/Doxygen.cmake
# ──────────────────────────────────────────────────────────────────────────────
# Helper function that creates a 'docs' target backed by Doxygen + Graphviz.
#
# Usage:
#   include(cmake/Doxygen.cmake)
#   calculator_add_doxygen(
#       TARGET        docs
#       INPUT_DIRS    "${PROJECT_SOURCE_DIR}/include"
#       OUTPUT_DIR    "${CMAKE_CURRENT_BINARY_DIR}/html"
#       DOXYFILE_IN   "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
#       DOXYFILE_OUT  "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
#       PROJECT_NAME  "${PROJECT_NAME}"
#       PROJECT_VERSION "${PROJECT_VERSION}"
#   )
# ──────────────────────────────────────────────────────────────────────────────
function(calculator_add_doxygen)
    cmake_parse_arguments(
        ARG                          # prefix
        ""                           # options
        "TARGET;OUTPUT_DIR;DOXYFILE_IN;DOXYFILE_OUT;PROJECT_NAME;PROJECT_VERSION"
        "INPUT_DIRS"                 # multi-value
        ${ARGN}
    )

    # ── Locate Doxygen ────────────────────────────────────────────────────────
    find_package(Doxygen OPTIONAL_COMPONENTS dot)

    if(NOT DOXYGEN_FOUND)
        message(WARNING
            "[Doxygen] Doxygen not found — documentation target '${ARG_TARGET}' "
            "will not be created. Install Doxygen to enable docs generation."
        )
        return()
    endif()

    if(NOT DOXYGEN_DOT_FOUND)
        message(WARNING
            "[Doxygen] Graphviz 'dot' not found — call-graphs and diagrams "
            "will be disabled."
        )
        set(DOXYGEN_HAVE_DOT "NO")
    else()
        set(DOXYGEN_HAVE_DOT "YES")
        message(STATUS "[Doxygen] Graphviz dot found: ${DOXYGEN_DOT_EXECUTABLE}")
    endif()

    message(STATUS "[Doxygen] Doxygen found: ${DOXYGEN_EXECUTABLE}")

    # ── Join INPUT_DIRS into a space-separated string for Doxyfile ────────────
    string(REPLACE ";" " " DOXY_INPUT_DIRS "${ARG_INPUT_DIRS}")

    # ── Variables substituted into Doxyfile.in ────────────────────────────────
    set(DOXY_PROJECT_NAME    "${ARG_PROJECT_NAME}")
    set(DOXY_PROJECT_VERSION "${ARG_PROJECT_VERSION}")
    set(DOXY_INPUT           "${DOXY_INPUT_DIRS}")
    set(DOXY_OUTPUT_DIR      "${ARG_OUTPUT_DIR}")
    set(DOXY_HAVE_DOT        "${DOXYGEN_HAVE_DOT}")
    set(DOXY_DOT_PATH        "${DOXYGEN_DOT_EXECUTABLE}")

    configure_file("${ARG_DOXYFILE_IN}" "${ARG_DOXYFILE_OUT}" @ONLY)

    # ── Custom target ─────────────────────────────────────────────────────────
    add_custom_target(
        "${ARG_TARGET}"
        COMMAND "${DOXYGEN_EXECUTABLE}" "${ARG_DOXYFILE_OUT}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating API documentation with Doxygen..."
        VERBATIM
    )

    # Place in a logical IDE folder
    set_target_properties("${ARG_TARGET}" PROPERTIES FOLDER "Documentation")

    message(STATUS
        "[Doxygen] Target '${ARG_TARGET}' created. "
        "Run: cmake --build <build-dir> --target ${ARG_TARGET}"
    )
endfunction()
