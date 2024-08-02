#ifndef INC_GY85_H_
#define INC_GY85_H_

#include <stdint.h>
#include "main.h"
#include "IIR_lowpass.h"

#define ADXL345             (0x53 << 1)     /* I2C address of accelerometer */
#define ADXL345_BW_RATE     (0x2C)          /* Register controls low power and ODR/Bandwidth */
#define ADXL345_RATE_CFG    (0x0B)          /* Set output data rate to 200 Hz */
#define ADXL345_POWER_CTL   (0x2D)          /* Register controls power settings*/
#define ADXL345_EN_MSR      (0x08)          /* Enable measurement mode */
#define ADXL345_DATA_FMT    (0x31)          /* Register controls representation of data */
#define ADXL345_FMT         (0x09)          /* Enable +-4g representation,
                                               full resolution and right-justified mode */
#define ADXL345_DATA        (0x32)          /* First accelerometer output register  */
#define ADXL345_OUT_LEN     (0x06)          /* Length of all output registers */


#define ITG3205             (0x68 << 1)     /* I2C address of gyroscope */
#define ITG3205_SMPLRT_DIV  (0x15)          /* Register controls sample rate divider */
#define ITG3205_DIV         (0x04)          /* Output sampling frequency - 200 Hz */
#define ITG3205_DLPF_FS     (0x16)          /* Register controls internal digital low-pass filter
                                               and full-scale range */
#define ITG3205_FS_CFG      (0x1D)          /* Value for proper gyro operation, low-pass bandwidth - 10 Hz,
                                               internal ADC sampling rate - 1kHz */
#define ITG3205_INT         (0x17)          /* Interrupt configuration register */
#define ITG3205_DIS_INT     (0x00)          /* Disables all interrupts on gyro */
#define ITG3205_PWR         (0x3E)          /* Register for power management */
#define ITG3205_PWR_CFG     (0x00)          /* Power management value - use internal oscillator */
#define ITG3205_DATA        (0x1D)          /* First gyro output register */
#define ITG3205_OUT_LEN     (0x06)          /* Length of all output registers */
#define ITG3205_NUM_SAMPLES (20)            /* Number of test samples used to account for gyroscope offset */


#define HMC5883             (0x1E << 1)     /* I2C address of a magnetometer */
#define HMC5883_MODEREG     (0x02)          /* Register that controls device operation mode */
#define HMC5883_MODECFG     (0x00)          /* Continuous measurement mode */
#define HMC5883_DATA        (0x03)          /* First magnetometer output register */
#define HMC5883_OUT_LEN     (0x06)          /* Length of all output registers */


/* Operational frequencies of devices */
#define ADXL345_FREQ        (200)
#define ITG3205_FREQ        (1000)
#define HMC58883_FREQ       (75)
#define SAMPLING_FREQ       (200)           /* We don't use magnetometer right now so frequency may be 200 Hz */
#define SAMPLING_PERIOD     (5)             /* Value in ms */


/* Conversion constants */
#define ACC_SCALE           (0.004)         /* g per LSB */
#define GYRO_SCALE          (1.0 / 14.375)  /* Degrees per second per LSB */
#define COMPASS_SCALE       (1090)          /* Gauss per LSB */


#define I2C_MEM_ADDR_SIZE_8BIT  (1u)

/*
 * struct GY85 - Contains all data related to GY85
 * @acc_x/y/z - Acceleromter measurements of x, y, z axis
 * @gyro_x/y/z - Gyro measurements
 * @compass_x/y/z - Magnetometer measurements
 * @hi2c - Pointer to I2C handle, which is used to communicate with the IMU
 * @huart - Pointer to UART handle, which is used to communicate with the host
 * @filter_acc - Acceletometer measurements lowpass filter
 * @filter_compass - Magnetometer measurements lowpass filter
 * */
struct GY85 {
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t compass_x;
    int16_t compass_y;
    int16_t compass_z;
    I2C_HandleTypeDef *hi2c;
    UART_HandleTypeDef *huart;
    struct IIR_lowpass *filter_acc;
    struct IIR_lowpass *filter_compass;
};

typedef enum {
    SENSOR_OK,
    SENSOR_TX_ERR,
    SENSOR_RX_ERR,
    SENSOR_INIT_ERR
} sensor_state;

/*
 * init_gy85 - Initialize GY85 IMU, which consists of initializing accelerometer, gyro and magnetometer
 * @gy85: Pointer to the IMU struct, which will be initialized. Must already contain pointers to I2C and UART handles
 *
 * Returns SENSOR_OK on success, SENSOR_INIT_ERR if UART or I2C handles are NULL, SENSOR_TX_ERR if I2C fails to transmit
 * */
sensor_state init_gy85(struct GY85 *gy85);

/*
 * read_acc - Read three accelerometer measured axis
 * @gy85: Pointer to the IMU struct, which manages this accelerometer
 *
 * Returns SENSOR_OK on success, SENSOR_INIT_ERR if UART or I2C handles are NULL, SENSOR_TX_ERR/SENSOR_RX_ERR if I2C fails to transmit/receive
 * */
sensor_state read_acc(struct GY85 *gy85);

/*
 * read_gyro - Read three gyroscope measured axis
 * @gy85: Pointer to the IMU struct, which manages this gyro
 *
 * Returns the same as read_acc
 * */
sensor_state read_gyro(struct GY85 *gy85);

/*
 * read_compass - Read three magnetometer measured axis
 * @gy85: Pointer to the IMU struct, which manages this compass
 *
 * Returns the same as read_acc
 * */
sensor_state read_compass(struct GY85 *gy85);

/*
 * transmit_readings - Transmit all readings using UART
 * @gy85: Pointer to the IMU struct, which manages this compass
 * @buf - Pointer to buffer that contains the message
 * @len - Length of the message to transmit
 *
 * Returns HAL_OK if message was successfully transmitted, HAL_ERROR otherwise
 * */
HAL_StatusTypeDef transmit_readings(struct GY85 *gy85, uint8_t *buf, int len);

#endif /* INC_GY85_H_ */
