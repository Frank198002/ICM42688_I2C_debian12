

#include "ICM42688.h"
#include <iostream>
#include <unistd.h>

int main() {

    ICM42688 imu("/dev/i2c-8", 0x68);

    int status = imu.begin();
    if (status < 0) {
        std::cout << "IMU init failed: " << status << std::endl;
        return -1;
    }

    //gyrp bias calibr
    float gyroBias[3];
    std::cout <<"Keep IMU still... calibrating...\n" << std::endl;
    imu.calibrateGyroBias(gyroBias);
    // 👉 打印零偏
    std::cout << "Gyro Bias:\n";
    std::cout << "X: " << gyroBias[0] << "  Y: " << gyroBias[1] << "  Z: " << gyroBias[2] << std::endl;

    while (1) {
        imu.getAGT();
        float gx = imu.gyrX() - gyroBias[0];
        float gy = imu.gyrY() - gyroBias[1];
        float gz = imu.gyrZ() - gyroBias[2];

        std::cout
            << imu.accX() << "\t"
            << imu.accY() << "\t"
            << imu.accZ() << "\t"
            << gx << "\t"
            << gy << "\t"
            << gz << std::endl;

        usleep(100000);
    }
}



// g++ main.cpp linux_i2c.cpp ICM42688.c -o imu_42688_linux \
//     -Iimu -I. \
//     -std=c++11 \
//     -lm -pthread

// tar -czvf ICM42688.tar.gz  /home/radxa/Workspace/ICM42688