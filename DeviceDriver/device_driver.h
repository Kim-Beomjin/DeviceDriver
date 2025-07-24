#pragma once
#include "flash_memory_device.h"

class DeviceDriver
{
public:
    DeviceDriver(FlashMemoryDevice* hardware);
    int read(long address);
    void write(long address, int data);

protected:
    FlashMemoryDevice* m_hardware;
};

class Application {
public:
    Application(DeviceDriver* deviceDriver) : deviceDriver{ deviceDriver } {}

    void readAndPrint(long startAddr, long endAddr) {
        for (long address = startAddr; address <= endAddr; address++) {
            deviceDriver->read(address);
        }
    }

private:
    DeviceDriver* deviceDriver;
};