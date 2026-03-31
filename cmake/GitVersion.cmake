# Determines project version from the nearest git tag matching "v*".
# Sets CALCULATOR_VERSION to "MAJOR.MINOR.PATCH", falls back to "0.0.0".

find_package(Git QUIET)
if(Git_FOUND)
    execute_process(
        COMMAND           ${GIT_EXECUTABLE} describe --tags --match "v*" --abbrev=0
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE   _GIT_TAG
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(_GIT_TAG MATCHES "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+)")
        set(CALCULATOR_VERSION
            "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
    endif()
endif()
if(NOT CALCULATOR_VERSION)
    set(CALCULATOR_VERSION "0.0.0")
    message(WARNING "No git tag found — version set to ${CALCULATOR_VERSION}")
endif()
