#include "BleApplication.h"

int main(int argc, const char **argv)
try
{
    rsp::BleApplication app(argc, argv);

    return app.Run();
}
catch (const std::exception &e) {
    std::cerr << "FATAL: Unhandled exception: " << e.what() << std::endl;
    return rsp::BleApplication::cResultUnhandledError;
}
catch(...) {
    std::cerr << "FATAL: Unknown exception" << std::endl;
    return rsp::BleApplication::cResultUnhandledError;
}

/*
#include "simplebluez/Bluez.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

static SimpleBluez::Bluez bluez;
static std::atomic_bool async_thread_active = true;

void async_thread_function() {
    while (async_thread_active) {
        bluez.run_async();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}

int main(int argc, char* argv[])
{
    bluez.init();
    auto* async_thread = new std::thread(async_thread_function);

    auto adapters = bluez.get_adapters();
    std::cout << "The following adapters were found:" << std::endl;
    for (int i = 0; i < adapters.size(); i++) {
        std::cout << "[" << i << "] " << adapters[i]->identifier() << " [" << adapters[i]->address() << "]"
                  << std::endl;
    }

    return 0;
}
*/