// /*
//  * wiringPiI2C.c:
//  *	Simplified I2C access routines
//  *	Copyright (c) 2013 Gordon Henderson
//  ***********************************************************************
//  * This file is part of wiringPi:
//  *	https://projects.drogon.net/raspberry-pi/wiringpi/
//  *
//  *    wiringPi is free software: you can redistribute it and/or modify
//  *    it under the terms of the GNU Lesser General Public License as
//  *    published by the Free Software Foundation, either version 3 of the
//  *    License, or (at your option) any later version.
//  *
//  *    wiringPi is distributed in the hope that it will be useful,
//  *    but WITHOUT ANY WARRANTY; without even the implied warranty of
//  *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  *    GNU Lesser General Public License for more details.
//  *
//  *    You should have received a copy of the GNU Lesser General Public
//  *    License along with wiringPi.
//  *    If not, see <http://www.gnu.org/licenses/>.
//  ***********************************************************************
//  */

// /*
//  * Notes:
//  *	The Linux I2C code is actually the same (almost) as the SMBus code.
//  *	SMBus is System Management Bus - and in essentially I2C with some
//  *	additional functionality added, and stricter controls on the electrical
//  *	specifications, etc. however I2C does work well with it and the
//  *	protocols work over both.
//  *
//  *	I'm directly including the SMBus functions here as some Linux distros
//  *	lack the correct header files, and also some header files are GPLv2
//  *	rather than the LGPL that wiringPi is released under - presumably because
//  *	originally no-one expected I2C/SMBus to be used outside the kernel -
//  *	however enter the Raspberry Pi with people now taking directly to I2C
//  *	devices without going via the kernel...
//  *
//  *	This may ultimately reduce the flexibility of this code, but it won't be
//  *	hard to maintain it and keep it current, should things change.
//  *
//  *	Information here gained from: kernel/Documentation/i2c/dev-interface
//  *	as well as other online resources.
//  *********************************************************************************
//  */
// #include <stdio.h>
// #include <stdlib.h>
// #include <stdint.h>
// #include <errno.h>
// #include <string.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/ioctl.h>
// #include <linux/i2c-dev.h>
// #include <linux/i2c.h>

// #include "linux_i2c.h"



// struct linux_i2c_ctx {
//     int fd;
//     uint8_t addr;
// };



// static int linux_i2c_write(inv_imu_serif *serif,
//                             uint8_t reg,
//                             const uint8_t *wbuffer,
//                             uint32_t wlen)
// {
//     linux_i2c_ctx *ctx = (linux_i2c_ctx *)serif->context;

//     uint8_t buf[256];
//     if (wlen + 1 > sizeof(buf)) return -1;

//     buf[0] = reg;
//     memcpy(&buf[1], wbuffer, wlen);

//     if (write(ctx->fd, buf, wlen + 1) != (int)(wlen + 1))
//         return -1;

//     return 0;
// }

// static int linux_i2c_read(inv_imu_serif *serif,
//                            uint8_t reg,
//                            uint8_t *rbuffer,
//                            uint32_t rlen)
// {
//     linux_i2c_ctx *ctx = (linux_i2c_ctx *)serif->context;

//     if (write(ctx->fd, &reg, 1) != 1)
//         return -1;

//     if (read(ctx->fd, rbuffer, rlen) != (int)rlen)
//         return -1;

//     return 0;
// }



// int linux_i2c_setup(inv_imu_serif *serif, const char *dev, uint8_t addr)
// {
//     linux_i2c_ctx *ctx = new linux_i2c_ctx();

//     ctx->fd = open(dev, O_RDWR);
//     if (ctx->fd < 0) {
//         perror("open i2c");
//         return -1;
//     }

//     if (ioctl(ctx->fd, I2C_SLAVE, addr) < 0) {
//         perror("ioctl i2c");
//         return -1;
//     }

//     serif->context   = ctx;
//     serif->serif_type = UI_I2C;
//     serif->read_reg   = linux_i2c_read;
//     serif->write_reg  = linux_i2c_write;

//     serif->max_read   = 256;
//     serif->max_write  = 256;

//     return 0;
// }


// int linux_i2c_close(inv_imu_serif *serif)
// {
//     if (!serif || !serif->context)
//         return -1;

//     linux_i2c_ctx *ctx = (linux_i2c_ctx *)serif->context;

//     if (ctx->fd > 0)
//         close(ctx->fd);

//     delete ctx;

//     serif->context = nullptr;
//     serif->read_reg = nullptr;
//     serif->write_reg = nullptr;

//     return 0;
// }


#include "linux_i2c.h"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

LinuxI2C::LinuxI2C(const char* device, uint8_t address)
{
    addr = address;

    fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("Failed to open i2c device");
        return;
    }

    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("Failed to set I2C address");
    }
}

LinuxI2C::~LinuxI2C()
{
    if (fd >= 0) close(fd);
}

int LinuxI2C::writeBytes(uint8_t reg, uint8_t* data, int len)
{
    uint8_t buf[32];

    if (len + 1 > 32) return -1;

    buf[0] = reg;
    memcpy(&buf[1], data, len);

    return write(fd, buf, len + 1);
}

int LinuxI2C::readBytes(uint8_t reg, uint8_t* data, int len)
{
    if (write(fd, &reg, 1) != 1)
        return -1;

    return read(fd, data, len);
}
