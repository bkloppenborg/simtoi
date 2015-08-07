# Set bogus values to these variables in case the git execution
# fails
SET(${GIT_VERSION_TAG} "v0.0.0")
SET(${GIT_VERSION_HASH} "000000")

# Create a file containing version information
EXECUTE_PROCESS(
    COMMAND git describe --abbrev=0 --tags
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
EXECUTE_PROCESS(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

MESSAGE(STATUS "Version information for" ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "Version tag: " ${GIT_VERSION_TAG})
MESSAGE(STATUS "Version hash: " ${GIT_COMMIT_HASH})

# strip the leading 'v' off of the git version tag:
string(LENGTH ${GIT_VERSION_TAG} GIT_TAG_LENGTH)
string(SUBSTRING ${GIT_VERSION_TAG} 1 -1 GIT_VERSION_TAG)

CONFIGURE_FILE(
    ${PROJECT_SOURCE_DIR}/src/version.h.in
    ${PROJECT_SOURCE_DIR}/src/version.h
)
