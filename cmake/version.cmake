set(_VERSION_REGEX ".*#define[ \t]+TINYTEX_VERSION_(.+)[ \t]+([0-9]+).*")

# read versions from version.h
file(
    STRINGS ${PROJECT_SOURCE_DIR}/lib/version.h
    _VERSION_STRINGS REGEX ${_VERSION_REGEX}
)

# pick out the sub version
function(FIND_VERSION SUB OUTPUT)
    string(
        REGEX REPLACE ".*#define[ \t]+TINYTEX_VERSION_${SUB}[ \t]+([0-9]+).*"
        "\\1"
        VERSION_SUB
        ${_VERSION_STRINGS}
    )
    set(${OUTPUT} ${VERSION_SUB} PARENT_SCOPE)
endfunction()

find_version(MAJOR VERSION_MAJOR)
find_version(MINOR VERSION_MINOR)
find_version(PATCH VERSION_PATCH)
find_version(NUMBER VERSION_NUMBER)

# set the version name
set(VERSION_NAME "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

message(STATUS "tinytex version name: ${VERSION_NAME}")
message(STATUS "tinytex version number: ${VERSION_NUMBER}")
