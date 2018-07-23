#include "atca_hal.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */


/** \brief This function delays for a number of microseconds.
 *
 * \param[in] delay number of microseconds to delay
 */

void atca_delay_us(uint32_t delay)
{
	vTaskDelay(delay / portTICK_PERIOD_MS/1000);
	// 1000/10 = 100 Ticks
}


/** \brief This function delays for a number of tens of microseconds.
 *
 * \param[in] delay number of 0.01 milliseconds to delay
 */

void atca_delay_10us(uint32_t delay)
{
	vTaskDelay(delay / portTICK_PERIOD_MS/100);
}

/** \brief This function delays for a number of milliseconds.
 *
 *         You can override this function if you like to do
 *         something else in your system while delaying.
 * \param[in] delay number of milliseconds to delay
 */

void atca_delay_ms(uint32_t delay)
{
	vTaskDelay(delay / portTICK_PERIOD_MS);
}

/** @} */
