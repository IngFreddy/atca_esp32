/**
 * \file
 *
 * \brief  Microchip Crypto Auth hardware interface object
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 * 
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef ATCA_IFACE_H
#define ATCA_IFACE_H

/** \defgroup interface ATCAIface (atca_)
 *  \brief Abstract interface to all CryptoAuth device types.  This interface
 *  connects to the HAL implementation and abstracts the physical details of the
 *  device communication from all the upper layers of CryptoAuthLib
   @{ */

#ifdef __cplusplus
extern "C" {
#endif

#include "atca_command.h"

typedef enum
{
    ATCA_I2C_IFACE,
    ATCA_SWI_IFACE,
    ATCA_UART_IFACE,
    ATCA_SPI_IFACE,
    ATCA_HID_IFACE,
    ATCA_CUSTOM_IFACE,
    // additional physical interface types here
    ATCA_UNKNOWN_IFACE,
} ATCAIfaceType;

/* ATCAIfaceCfg is a mediator object between a completely abstract notion of a
   physical interface and an actual physical interface.

   The main purpose of it is to keep hardware specifics from bleeding into the
   higher levels - hardware specifics could include things like framework
   specific items (ASF SERCOM) vs a non-Microchip I2C library constant that
   defines an I2C port. But I2C has roughly the same parameters regardless of
   architecture and framework.
 */

typedef struct
{

} ATCAI2C;

typedef struct
{

    ATCAIfaceType  iface_type;      // active iface - how to interpret the union below
    ATCADeviceType devtype;         // explicit device type
	uint8_t  slave_address; // 8-bit slave address
	uint32_t baud;          // typically 400000
    uint16_t wake_delay;    // microseconds of tWHI + tWLO which varies based on chip type
    int      rx_retries;    // the number of retries to attempt for receiving bytes
    void *   cfg_data;      // opaque data used by HAL in device discovery
} ATCAIfaceCfg;
typedef struct atca_iface * ATCAIface;


/** \brief atca_iface is the C object backing ATCAIface.  See the atca_iface.h file for
 * details on the ATCAIface methods
 */

struct atca_iface
{
    ATCAIfaceType mType;
    ATCAIfaceCfg *mIfaceCFG;    // points to previous defined/given Cfg object, caller manages this

    ATCA_STATUS (*atinit)(void *hal, ATCAIfaceCfg *);
    ATCA_STATUS (*atpostinit)(ATCAIface hal);
    ATCA_STATUS (*atsend)(ATCAIface hal, uint8_t *txdata, int txlength);
    ATCA_STATUS (*atreceive)(ATCAIface hal, uint8_t *rxdata, uint16_t *rxlength);
    ATCA_STATUS (*atwake)(ATCAIface hal);
    ATCA_STATUS (*atidle)(ATCAIface hal);
    ATCA_STATUS (*atsleep)(ATCAIface hal);

    // treat as private
    void *hal_data;     // generic pointer used by HAL to point to architecture specific structure
                        // no ATCA object should touch this except HAL, HAL manages this pointer and memory it points to
};

ATCA_STATUS initATCAIface(ATCAIfaceCfg *cfg, ATCAIface ca_iface);
ATCAIface newATCAIface(ATCAIfaceCfg *cfg);
ATCA_STATUS releaseATCAIface(ATCAIface ca_iface);
void deleteATCAIface(ATCAIface *ca_iface);

// IFace methods
ATCA_STATUS atinit(ATCAIface ca_iface);
ATCA_STATUS atpostinit(ATCAIface ca_iface);
ATCA_STATUS atsend(ATCAIface ca_iface, uint8_t *txdata, int txlength);
ATCA_STATUS atreceive(ATCAIface ca_iface, uint8_t *rxdata, uint16_t *rxlength);
ATCA_STATUS atwake(ATCAIface ca_iface);
ATCA_STATUS atidle(ATCAIface ca_iface);
ATCA_STATUS atsleep(ATCAIface ca_iface);

// accessors
ATCAIfaceCfg * atgetifacecfg(ATCAIface ca_iface);
void* atgetifacehaldat(ATCAIface ca_iface);


#ifdef __cplusplus
}
#endif
/** @} */
#endif


