# BluetoothGlucose
RSP developed C++ software for Bluetooth Low Energy (BLE) glucose profiles

### License
This software is open-source, released under the [Mozilla Public License v.2.0](https://www.mozilla.org/en-US/MPL/2.0/).

### Prerequisites
This software uses the cross-platform [SimpleBLE library](https://simpleble.readthedocs.io/en/latest/overview.html), it is only tested on Linux though.
Library installation:
```shell
sudo apt install libdbus-1-dev
```
The software also uses components from the [RSP Core Library](https://github.com/rsps/rsp-core-library), automatically included via cmake. 
Essential build tools like cmake and gcc are also needed.

## ble-dump
CLI application to connect and dump all records from a BGM supporting the Bluetooth Glucose Service profile.

ble-bump can list host adapter information, scan for devices, connect to an individual device and dump service profiles.
It can connect via either Bluetooth MAC address or device name, even partial names to connect to the first matching
device found.

### Make and installation
The software support out of tree builds using cmake.
First cd  into source directory, then: 
```shell
mkdir build
cd build
cmake -G Ninja ..
cmake --build .
sudo cmake --install .
```

### Usage
Show built-in help:
```shell
ble-dump --help
```
Scan for devices:
```shell
ble-dump --adapter=hci0 devices
```
List services on specific device:
```shell
ble-dump --adapter=hci1 --device=CC:78:AB:A3:F4:34 attributes
```
Dump Glucose Measurement records from first Contour Next One device found:
```shell
ble-dump --adapter=hci1 --device="Contour*" dump
```
Dump records in json format, default is csv:
```shell
ble-dump --adapter=hci1 --device="Contour*" --encoder=json dump
```
