include(FetchContent)
FetchContent_Declare(
        SimpleBluez
        GIT_REPOSITORY git@github.com:OpenBluetoothToolbox/SimpleBLE.git
        GIT_TAG v0.6.1
        GIT_SHALLOW YES
)

# Note that here we manually do what FetchContent_MakeAvailable() would do,
# except to ensure that the dependency can also get what it needs, we add
# custom logic between the FetchContent_Populate() and add_subdirectory()
# calls.
FetchContent_GetProperties(SimpleBluez)
if(NOT SimpleBluez_POPULATED)
    FetchContent_Populate(SimpleBluez)
    list(APPEND CMAKE_MODULE_PATH "${SimpleBluez_SOURCE_DIR}/cmake/find")
    add_subdirectory("${simplebluez_SOURCE_DIR}/simplebluez" "${SimpleBluez_BINARY_DIR}")
endif()

set(SimpleBluez_FOUND 1)