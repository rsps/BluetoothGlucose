include(FetchContent)
FetchContent_Declare(
        SimpleBLE
        GIT_REPOSITORY git@github.com:OpenBluetoothToolbox/SimpleBLE.git
        GIT_TAG v0.7.1
        GIT_SHALLOW YES
)

# Note that here we manually do what FetchContent_MakeAvailable() would do,
# except to ensure that the dependency can also get what it needs, we add
# custom logic between the FetchContent_Populate() and add_subdirectory()
# calls.
FetchContent_GetProperties(SimpleBLE)
if(NOT simpleble_POPULATED)
    FetchContent_Populate(SimpleBLE)
    list(APPEND CMAKE_MODULE_PATH "${simpleble_SOURCE_DIR}/cmake/find")
    add_subdirectory("${simpleble_SOURCE_DIR}/simpleble" "${simpleble_BINARY_DIR}")
endif()

set(simpleble_FOUND 1)