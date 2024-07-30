#ifndef INC_ATTITUDE_COMPL_H_
#define INC_ATTITUDE_COMPL_H_

#include "GY85.h"

#define COMPL_ALPHA         (0.1)
#define RAD_TO_DEG          (57.2958f)
#define DEG_TO_RAD          (1.0f / RAD_TO_DEG)

/*
 * compute_attitude - Calculate roll and pitch angles from accelerometer and gyro measurements.
 * Output is dependent on COMPL_ALPHA which regulates what sensor will receive more trust.
 *
 * @out - Array where computed roll and pitch will be written
 * @imu - IMU that manages GY85
 * */
void compute_attitude(float *out, struct GY85 *imu);

#endif /* INC_ATTITUDE_COMPL_H_ */
