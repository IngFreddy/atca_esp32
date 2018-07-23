#ifndef HAL_ESP32_I2C
#define HAL_ESP32_I2C


#include <stdint.h>
#include <sys/types.h>
#include <driver/i2c.h>
#include <driver/gpio.h>


static const gpio_num_t ESP_I2C_SDA_PIN = (gpio_num_t)21;
static const gpio_num_t ESP_I2C_SCL_PIN = (gpio_num_t)22;

typedef struct atcaI2Cmaster
{
    int ref_ct;
    int state;
} ATCAI2CMaster_t;

void change_i2c_speed(ATCAIface iface, uint32_t speed);

/** @} */

#endif
