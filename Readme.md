# GY85 IMU Attitude Estimation Complementary Filter

## Overview

This repository is an implementation of orientation(Euler angles) estimation based on IMU data, which is fused using complementary filter technique.

## Credits

The project is based on Youtube [Phil's Lab](https://www.youtube.com/watch?v=RZd6XDx5VXo&list=PLXSyc11qLa1ZCn0JCnaaXOWN6Z46rK9jd&index=6) videos on the topic.
Also for magnetometer estimations of yaw angle [this paper](https://ietresearch.onlinelibrary.wiley.com/doi/full/10.1049/trit.2017.0024) was used

## Features

- Contains complete driver for communication with GY85 sensor board, e. g. can communicate with ADXL345, ITG3205 and HMC5883L
- Accelerometer model assumes no linear and rotational accelerations, only gravity
- To find orientation based on gyroscope the integration is applied to measured roll rates. The gyro drift is eliminated using complementary filter, which also considers orientation from the accelerometer model
- EMA IIR filter is applied to accelerometer measurements and output data
- Repo contains python roll, pitch and yaw angles graph builder
- Code uses simple `sprintf` implementation named `strprint()` to copy formatted data to the buffer. This function only supports ***%d*** and ***%f*** formats

## Usage

- This project isn't intended to be built, rather just copied in `Src` and `Inc` directories of STM32 project
- `main.c` contains example of heading estimation with the known sampling frequency
- `init_gy85` is used to initialize IMU, then `read_gyro()`, `read_acc()` and `read_compass()` are used to read the measurements from the GY85
- `struct IIR_lowpass` from *IIR_lowpass.h* to filter data and `strprint` from *strprint.h* to copy formatted data
- Use `compute_attitude()` from *attitude_compl.h* to find roll, pitch and yaw data from complementary filter

## Issues

- For some reason yaw angle changes with roll or pitch, presumably it is happening because of Euler angles interdependence

