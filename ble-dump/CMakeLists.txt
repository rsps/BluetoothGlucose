set(APP_NAME "ble-dump")

add_executable(${APP_NAME}
        main.cpp
        BleApplication.cpp
        UUID.cpp
        TrustedDevice.cpp
)

add_dependencies(${APP_NAME} rsp-core-lib)

target_include_directories(${APP_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/ble-dump
)

target_link_libraries(${APP_NAME}
        rsp-core-lib
        simplebluez::simplebluez
)

#if (BUILD_TESTING)
#    add_subdirectory(tests)
#endif()

install(TARGETS ${APP_NAME} DESTINATION )

