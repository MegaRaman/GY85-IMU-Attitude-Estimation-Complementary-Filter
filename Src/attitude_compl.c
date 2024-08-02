#include <math.h>
#include "attitude_compl.h"

static float prev_roll;
static float prev_pitch;
static float prev_yaw;


/* convert measurements, transform to eulers rates and integrate them */
void gyro_estimate_attitude(float *out_attitude, struct GY85 *imu) {
    float p = imu->gyro_x * GYRO_SCALE * DEG_TO_RAD;
    float q = imu->gyro_y * GYRO_SCALE * DEG_TO_RAD;
    float r = imu->gyro_z * GYRO_SCALE * DEG_TO_RAD;

    float pitch_tan;

    if (prev_pitch >= M_PI / 2 - 0.1 && prev_pitch <= M_PI / 2 + 0.1) {
        pitch_tan = 1;
    }
    else if (prev_pitch >= -M_PI / 2 - 0.1 && prev_pitch <= -M_PI / 2 + 0.1) {
        pitch_tan = -1;
    }
    else {
        pitch_tan = tanf(prev_pitch);
    }

    float roll_rate = p + pitch_tan * (sinf(prev_roll) * q + cosf(prev_roll) * r);
    float pitch_rate = cosf(prev_roll) * q - sinf(prev_roll) * r;
    float yaw_rate = (sin(prev_roll) * q + cos(prev_roll) * r) / cos(prev_pitch);

    out_attitude[0] = prev_roll + SAMPLING_PERIOD / 1000.0f * roll_rate;
    out_attitude[1] = prev_pitch + SAMPLING_PERIOD / 1000.0f * pitch_rate;
    out_attitude[2] = prev_yaw + SAMPLING_PERIOD / 1000.0f * yaw_rate;

}

void acc_estimate_attitude(float *out_attitude, struct GY85 *imu) {
    /* Compute roll estimate as arctan(ay / az) */
    out_attitude[0] = atanf(imu->filter_acc->out[1] / imu->filter_acc->out[2]);

    /* Compute pitch estimate as arcsin(ax / g), ax can be represented in g so no need to convert to m/s^2 and divide by 9.81 */
    out_attitude[1] = asinf(imu->filter_acc->out[0] * ACC_SCALE);

    if (isnan(out_attitude[1])) {
        if (imu->filter_acc->out[0] * ACC_SCALE < -1)
            out_attitude[1] = -M_PI / 2;
        else
            out_attitude[1] = M_PI / 2;
    }

    float magnetometer_x = imu->filter_compass->out[0] * cosf(prev_pitch) +
            imu->filter_compass->out[1] * sinf(prev_pitch) * sinf(prev_roll) +
            imu->filter_compass->out[2] * sinf(prev_pitch) * cosf(prev_roll);
    float magnetometer_y = imu->filter_compass->out[1] * cosf(prev_roll) - imu->filter_compass->out[2] * sinf(prev_roll);

    float yaw_arctan = -atanf(magnetometer_y / magnetometer_x);

    if (magnetometer_x > 0 && magnetometer_y <= 0) {
        out_attitude[2] = yaw_arctan;
    }
    else if (magnetometer_x == 0 && magnetometer_y < 0) {
        out_attitude[2] = M_PI / 2;
    }
    else if (magnetometer_x < 0) {
        out_attitude[2] = M_PI + yaw_arctan;
    }
    else if (magnetometer_x == 0 && magnetometer_y > 0) {
        out_attitude[2] = 3 * M_PI / 2;
    }
    else {
        out_attitude[2] = 2 * M_PI + yaw_arctan;
    }
}

/* out = (1 - alpha) * gyro_est + alpha * accelerometer_est */
void compute_attitude(float *out, struct GY85 *imu) {
    float gyro_estimates[3];
    float acc_estimates[3];

    gyro_estimate_attitude(gyro_estimates, imu);
    acc_estimate_attitude(acc_estimates, imu);

    out[0] = (1 - COMPL_ALPHA) * gyro_estimates[0] + COMPL_ALPHA * acc_estimates[0];
    out[1] = (1 - COMPL_ALPHA) * gyro_estimates[1] + COMPL_ALPHA * acc_estimates[1];
    out[2] = (1 - COMPL_ALPHA) * gyro_estimates[2] + COMPL_ALPHA * acc_estimates[2];

    prev_roll = out[0];
    prev_pitch = out[1];
    prev_yaw = out[2];

    out[0] *=  RAD_TO_DEG;
    out[1] *=  RAD_TO_DEG;
    out[2] *= RAD_TO_DEG;
}
