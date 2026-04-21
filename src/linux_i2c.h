#pragma once
#include <stdint.h>

class LinuxI2C {
public:
    LinuxI2C(const char* device, uint8_t address);
    ~LinuxI2C();

    int writeBytes(uint8_t reg, uint8_t* data, int len);
    int readBytes(uint8_t reg, uint8_t* data, int len);

    int fd;
    uint8_t addr;
};

