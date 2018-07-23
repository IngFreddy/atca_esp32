
#include <string.h>
#include <stdio.h>

#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <stdint.h>
#include <sys/types.h>
#include <esp_log.h>

#include "atca_hal.h"
#include "atca_device.h"

#include "hal_esp32_i2c.h"
#include "atca_execution.h"

#define ACK_CHECK_EN    0x1
#define ACK_VAL     	0x1
#define NACK_VAL        0x0


ATCAI2CMaster_t *i2c_hal_data;
int i2c_bus_ref_ct = 0;

static const char* LOG_TAG = "I2C_HAL";

ATCA_STATUS hal_i2c_init(void *hal, ATCAIfaceCfg *cfg)
{
	if (i2c_bus_ref_ct == 0)
	{
		i2c_bus_ref_ct++;

		i2c_hal_data = malloc(sizeof(ATCAI2CMaster_t) );
		i2c_hal_data->ref_ct = 1;
		i2c_hal_data->state = 0;

		i2c_config_t conf;
		conf.mode             = I2C_MODE_MASTER;
		conf.sda_io_num       = ESP_I2C_SDA_PIN;
		conf.scl_io_num       = ESP_I2C_SCL_PIN;
		conf.sda_pullup_en    = GPIO_PULLUP_ENABLE;
		conf.scl_pullup_en    = GPIO_PULLUP_ENABLE;
		conf.master.clk_speed = cfg->baud;

		esp_err_t errRc = i2c_param_config(I2C_NUM_0, &conf);
		if (errRc != ESP_OK) {
			ESP_LOGE(LOG_TAG, "i2c_param_config: rc=%d", errRc);
			return ATCA_COMM_FAIL;
		}
		errRc = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
		if (errRc != ESP_OK) {
			ESP_LOGE(LOG_TAG, "i2c_driver_install: rc=%d", errRc);
			return ATCA_COMM_FAIL;
		}

		((ATCAHAL_t*)hal)->hal_data = i2c_hal_data;
	}
    else
    {
        i2c_hal_data->ref_ct++;
    }

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_send(ATCAIface iface, uint8_t *txdata, int txlength)
{
	ATCAIfaceCfg *cfg = atgetifacecfg(iface);
	uint8_t address = cfg->slave_address;

	txdata[0] = 0x03;
	txlength++;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	esp_err_t errRc = i2c_master_write_byte(cmd, address | I2C_MASTER_WRITE, ACK_CHECK_EN);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "i2c_master_1write_byte: rc=%d", errRc);
		return ATCA_COMM_FAIL;
	}
	if(txlength > 0){
		errRc = i2c_master_write(cmd, txdata, txlength, ACK_CHECK_EN);
		if (errRc != ESP_OK) {
			ESP_LOGE(LOG_TAG, "i2c_master_2write_byte: rc=%d", errRc);
			return ATCA_COMM_FAIL;
		}
	}

	vTaskDelay(10 / portTICK_PERIOD_MS);

	i2c_master_stop(cmd);
	errRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1500 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "i2c_master_3write_byte: rc=%d", errRc);
		return ATCA_COMM_FAIL;
	}


	return ATCA_SUCCESS;
}


ATCA_STATUS hal_i2c_receive(ATCAIface iface, uint8_t *rxdata, uint16_t *rxlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    uint8_t address = cfg->slave_address;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
	esp_err_t errRc = i2c_master_write_byte(cmd, address | I2C_MASTER_READ, ACK_CHECK_EN);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "i2c_master_4write_byte: rc=%d", errRc);
		return ATCA_COMM_FAIL;
	}
    if (*rxlength > 1) {
        i2c_master_read(cmd, rxdata, *rxlength - 1, (i2c_ack_type_t)NACK_VAL);
    }
    errRc = i2c_master_read_byte(cmd, rxdata + *rxlength - 1, (i2c_ack_type_t)ACK_VAL);
    i2c_master_stop(cmd);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "i2c_master_5write_byte: rc=%d", errRc);
		return ATCA_COMM_FAIL;
	}

	vTaskDelay(10 / portTICK_PERIOD_MS);

    errRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1500 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	if (errRc != ESP_OK) {
		ESP_LOGE(LOG_TAG, "i2c_master_6write_byte: rc=%d", errRc);
		return ATCA_COMM_FAIL;
	}

    return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_wake(ATCAIface iface)
{
	if(i2c_hal_data->state == 0){
		ESP_LOGW(LOG_TAG,"WAKEUP");

		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		esp_err_t errRc = i2c_master_write_byte(cmd, 0 | I2C_MASTER_WRITE, 0);
		i2c_master_stop(cmd);
		i2c_cmd_link_delete(cmd);

		uint8_t data[4], expected[4] = { 0x04, 0x11, 0x33, 0x43 };
		uint16_t dataL = 4;

		for(int i=0; hal_i2c_receive(iface, data, &dataL) != 0 && i<15; i++){
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		if (memcmp(data, expected, 4) == 0)
		{
			i2c_hal_data->state++;
			return ATCA_SUCCESS;
		}
		ESP_LOGW(LOG_TAG, "0x%02x, 0x%02x, 0x%02x, 0x%02x",data[0],data[1],data[2],data[3]);

		return ATCA_COMM_FAIL;

	}

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_idle(ATCAIface iface)
{
	/*ATCAIfaceCfg *cfg = atgetifacecfg(iface);
	uint8_t address = cfg->slave_address;

	ESP_LOGW(LOG_TAG,"IDLE");
	uint8_t data = 0x02;	// idle word address value

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	esp_err_t errRc = i2c_master_write_byte(cmd, address | I2C_MASTER_WRITE, ACK_CHECK_EN);
	errRc = i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);

	errRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1500 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);*/

	return ATCA_SUCCESS;
}

ATCA_STATUS hal_i2c_sleep(ATCAIface iface)
{
	ATCAIfaceCfg *cfg = atgetifacecfg(iface);
	uint8_t address = cfg->slave_address;

	i2c_hal_data->state = 0;

	ESP_LOGW(LOG_TAG,"SLEEP");
	uint8_t data = 0x01;	// idle word address value

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);

	esp_err_t errRc = i2c_master_write_byte(cmd, address | I2C_MASTER_WRITE, ACK_CHECK_EN);
	errRc = i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);

	errRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1500 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ATCA_SUCCESS;
}

/** \brief manages reference count on given bus and releases resource if no more refences exist
 * \param[in] hal_data - opaque pointer to hal data structure - known only to the HAL implementation
 * return ATCA_SUCCESS
 */

ATCA_STATUS hal_i2c_release(void *hal_data)
{
    ATCAI2CMaster_t *hal = (ATCAI2CMaster_t*)hal_data;

    i2c_bus_ref_ct--;  // track total i2c bus interface instances for consistency checking and debugging

    // if the use count for this bus has gone to 0 references, disable it.  protect against an unbracketed release
    if (hal && --(hal->ref_ct) <= 0 && i2c_hal_data != NULL)
    {
        free(i2c_hal_data);
        i2c_hal_data = NULL;
    }

    return ATCA_SUCCESS;
}
