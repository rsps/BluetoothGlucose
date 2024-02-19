include(FetchContent)
FetchContent_Declare(
        simpleble
        GIT_REPOSITORY git@github.com:OpenBluetoothToolbox/SimpleBLE.git
        GIT_TAG v0.7.1
        GIT_SHALLOW YES
)

# Note that here we manually do what FetchContent_MakeAvailable() would do,
# except to ensure that the dependency can also get what it needs, we add
# custom logic between the FetchContent_Populate() and add_subdirectory()
# calls.
FetchContent_GetProperties(simpleble)
if(NOT simpleble_POPULATED)
    FetchContent_Populate(simpleble)
    list(APPEND CMAKE_MODULE_PATH "${simpleble_SOURCE_DIR}/cmake/find")
    add_subdirectory("${simpleble_SOURCE_DIR}/simpleble" "${simpleble_BINARY_DIR}")
endif()

find_package(DBus1 REQUIRED)

set(simplebluez_inc
        ${simpleble_SOURCE_DIR}/simplebluez/include
        ${simpleble_SOURCE_DIR}/simpledbus/include
        ${DBus1_INCLUDE_DIR}
        ${DBus1_ARCH_INCLUDE_DIR}
)
set(simpleble_FOUND 1)