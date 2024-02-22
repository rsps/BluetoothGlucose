/**
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at https://mozilla.org/MPL/2.0/.
*
* \copyright   Copyright 2024 RSP Systems A/S. All rights reserved.
* \license     Mozilla Public License 2.0
* \author      steffen
*/

#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <logging/LogChannel.h>
#include <simpleble/SimpleBLE.h>

namespace rsp {

class Scanner : public logging::NamedLogger<Scanner>
{
public:
    using FilterList = std::vector<std::string>;

    explicit Scanner(const SimpleBLE::Adapter &arAdapter, FilterList aAddressList = {});
    explicit Scanner(const SimpleBLE::Adapter &arAdapter, const std::string &arAddress);

    const std::vector<SimpleBLE::Peripheral>& RunFor(std::uint32_t aMilliseconds);
    bool RunUntilFound(std::uint32_t aTimeoutMilliseconds);

    [[nodiscard]] const std::vector<SimpleBLE::Peripheral>& GetResult() const { return mScanResult; }

protected:
    SimpleBLE::Adapter mAdapter;
    FilterList mAcceptFilter{};
    std::vector<SimpleBLE::Peripheral> mScanResult{};
    bool mFoundDevice = false;

    void execute(std::uint32_t aMilliseconds, bool aStopWhenFound);
    [[nodiscard]] bool addressAccepted(SimpleBLE::Peripheral &arPeripheral) const;
};

} // rsp

#endif //SCANNER_H
