set(APP_NAME "ble-dump")

include(version)
version(0 1 1)

add_executable(${APP_NAME}
        main.cpp
        BleApplication.cpp
        UUID.cpp
        TrustedDevice.cpp
        GlucoseServiceProfile.cpp
        AttributeStream.cpp
        BleServiceBase.cpp
        Scanner.cpp
        DeviceInformationServiceProfile.cpp
        CurrentTimeServiceProfile.cpp
)

add_dependencies(${APP_NAME} rsp-core-lib)

target_include_directories(${APP_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include/${APP_NAME}
)

target_link_libraries(${APP_NAME}
        rsp-core-lib
        simpleble::simpleble
)

#if (BUILD_TESTING)
#    add_subdirectory(tests)
#endif()

install(TARGETS ${APP_NAME} DESTINATION )

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "${APP_NAME}")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_CONTACT "Steffen <steffen@rspsystems.com>")
set(CPACK_PACKAGE_DESCRIPTION "CLI application to connect and dump all records from a BGM supporting the Bluetooth Glucose Service profile.")
set(CPACK_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_PACKAGE_VERSION_MAJOR "${APP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${APP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${APP_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}_${CPACK_PACKAGE_ARCHITECTURE}")
include(CPack)
