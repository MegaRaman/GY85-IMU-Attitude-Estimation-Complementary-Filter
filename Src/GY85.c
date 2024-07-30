#include <string.h>
#include "GY85.h"


static int32_t x_offset;
static int32_t y_offset;
static int32_t z_offset;

sensor_state init_acc(struct GY85 *gy85) {
    uint8_t tmp = ADXL345_FMT;
    gy85->acc_x = gy85->acc_y = gy85->acc_z = 0;

    /* Configure accelerometer resolution and format */
    if (HAL_I2C_Mem_Write(gy85->hi2c, ADXL345, ADXL345_DATA_FMT, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    tmp = ADXL345_RATE_CFG;
    /* Set output data rate to 200 Hz */
    if (HAL_I2C_Mem_Write(gy85->hi2c, ADXL345, ADXL345_BW_RATE, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    tmp = ADXL345_EN_MSR;
    /* Enable measurements */
    if (HAL_I2C_Mem_Write(gy85->hi2c, ADXL345, ADXL345_POWER_CTL, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    return SENSOR_OK;
}

void calibrate_offset(struct GY85 *gy85);

sensor_state init_gyro(struct GY85 *gy85) {
    uint8_t tmp = ITG3205_DIV;
    gy85->gyro_x = gy85->gyro_y = gy85->gyro_z = 0;

    /* Configure gyroscope sample rate divider */
    if (HAL_I2C_Mem_Write(gy85->hi2c, ITG3205, ITG3205_SMPLRT_DIV, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    tmp = ITG3205_FS_CFG;

    /* Configure sampling rate and internal low-pass */
    if (HAL_I2C_Mem_Write(gy85->hi2c, ITG3205, ITG3205_DLPF_FS, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    tmp = ITG3205_DIS_INT;

    /* Disable gyroscope interrupts */
    if (HAL_I2C_Mem_Write(gy85->hi2c, ITG3205, ITG3205_INT, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    tmp = ITG3205_PWR_CFG;

    /* Select internal oscillator as clock source*/
    if (HAL_I2C_Mem_Write(gy85->hi2c, ITG3205, ITG3205_PWR, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    calibrate_offset(gy85);

    return SENSOR_OK;
}

sensor_state init_compass(struct GY85 *gy85) {
    uint8_t tmp = HMC5883_MODECFG;

    gy85->compass_x = gy85->compass_y = gy85->compass_z = 0;

    if (HAL_I2C_Mem_Write(gy85->hi2c, HMC5883, HMC5883_MODEREG, I2C_MEM_ADDR_SIZE_8BIT, &tmp, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    return SENSOR_OK;
}

sensor_state init_gy85(struct GY85 *gy85) {
    sensor_state ret = SENSOR_OK;
    if (gy85->hi2c == NULL || gy85->huart == NULL)
        return SENSOR_INIT_ERR;
    ret = init_acc(gy85);
    if (ret != SENSOR_OK)
        return ret;
    ret = init_gyro(gy85);
    if (ret != SENSOR_OK)
        return ret;
    ret = init_compass(gy85);
    if (ret != SENSOR_OK)
        return ret;

    return ret;
}

// TODO: must use dma
sensor_state read_acc(struct GY85 *gy85) {
    uint8_t reg_addr = ADXL345_DATA;
    uint8_t tmp[ADXL345_OUT_LEN];

    if (HAL_I2C_Master_Transmit(gy85->hi2c, ADXL345, &reg_addr, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    if (HAL_I2C_Master_Receive(gy85->hi2c, ADXL345, tmp, ADXL345_OUT_LEN, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_RX_ERR;

    gy85->acc_x = (tmp[1] << 8) | tmp[0];
    gy85->acc_y = (tmp[3] << 8) | tmp[2];
    gy85->acc_z = (tmp[5] << 8) | tmp[4];

    return SENSOR_OK;
}

sensor_state read_gyro(struct GY85 *gy85) {
    uint8_t reg_addr = ITG3205_DATA;
    uint8_t tmp[ITG3205_OUT_LEN];

    if (HAL_I2C_Master_Transmit(gy85->hi2c, ITG3205, &reg_addr, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    if (HAL_I2C_Master_Receive(gy85->hi2c, ITG3205, tmp, ITG3205_OUT_LEN, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_RX_ERR;

    gy85->gyro_x = ((tmp[0] << 8) | tmp[1]) - x_offset;
    gy85->gyro_y = ((tmp[2] << 8) | tmp[3]) - y_offset;
    gy85->gyro_z = ((tmp[4] << 8) | tmp[5]) - z_offset;
    return SENSOR_OK;
}

sensor_state read_compass(struct GY85 *gy85) {
    uint8_t reg_addr = HMC5883_DATA;
    uint8_t tmp[HMC5883_OUT_LEN];
    if (HAL_I2C_Master_Transmit(gy85->hi2c, HMC5883, &reg_addr, 1, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_TX_ERR;

    if (HAL_I2C_Master_Receive(gy85->hi2c, HMC5883, tmp, HMC5883_OUT_LEN, HAL_MAX_DELAY) != HAL_OK)
        return SENSOR_RX_ERR;

    gy85->compass_x = tmp[0] << 8 | tmp[1];
    gy85->compass_z = tmp[2] << 8 | tmp[3];
    gy85->compass_y = tmp[4] << 8 | tmp[5];

    return SENSOR_OK;
}

// TODO: must use dma
HAL_StatusTypeDef transmit_readings(struct GY85 *gy85, uint8_t *buf, int len) {
    HAL_StatusTypeDef ret;
    ret = HAL_UART_Transmit(gy85->huart, buf, len, HAL_MAX_DELAY);
    return ret;
}

/* Calibrate gyroscope measurement offset */
void calibrate_offset(struct GY85 *gy85) {
    int32_t x_tmp = 0;
    int32_t y_tmp = 0;
    int32_t z_tmp = 0;


    for (uint8_t i = 0; i < ITG3205_NUM_SAMPLES; i++) {
        HAL_Delay(50);
        read_gyro(gy85);
        x_tmp += gy85->gyro_x;
        y_tmp += gy85->gyro_y;
        z_tmp += gy85->gyro_z;
    }
    x_offset = x_tmp / ITG3205_NUM_SAMPLES;
    y_offset = y_tmp / ITG3205_NUM_SAMPLES;
    z_offset = z_tmp / ITG3205_NUM_SAMPLES;
}
