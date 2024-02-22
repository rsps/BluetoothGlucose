/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/
#include <BleApplication.h>

int main(int argc, const char **argv)
try
{
    rsp::BleApplication app(argc, argv);

    return app.Run();
}
catch (const std::exception &e) {
    std::cerr << "FATAL: " << e.what() << std::endl;
    return rsp::BleApplication::cResultUnhandledError;
}
catch(...) {
    std::cerr << "FATAL: An unknown error occurred." << std::endl;
    return rsp::BleApplication::cResultUnhandledError;
}
