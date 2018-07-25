/*
 * ATCA ATECC508A for ESP32
 * configure.h
 *
 * Created: 3. 7. 2018 13:46:20
 *  Author: Milan Mucka
 */

#ifndef ASYMETRIC_H_
#define ASYMETRIC_H_


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <esp_log.h>

#include <hwcrypto/aes.h>

#include "cryptoauthlib.h"
#include "atca_host.h"
#include "atca_basic.h"

#include "provision.h"


#define MSG_NONCE_SIZE	32
#define ATCA_HAL_I2C

#define ATECC508A_ADDRESS 	0xC0

#define ASYMETRIC_KEY_SLOT	4
#define TRANSPORT_KEY_SLOT	2

const ATCAIfaceCfg cfg_ateccx08a_i2c = {
	.iface_type			= ATCA_I2C_IFACE,
	.devtype			= ATECC508A,
	.slave_address		= ATECC508A_ADDRESS,
	.baud				= 100000,
	.wake_delay			= 1500,
	.rx_retries			= 20,
	.cfg_data 			= NULL
};

//key for hash secret
const uint8_t transport_key[] = {
	0xf2, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x11,
	0x11, 0x11, 0x11, 0x2f,
};


class Encryption{
public:
	Encryption();
	~Encryption();

	int setRemote(uint8_t* pubKey);
	int encryptData(uint8_t* input, uint8_t* output, size_t length);
	int decryptData(uint8_t* input, uint8_t* output, size_t length);
	uint8_t* getPublicKey();

private:
	uint8_t remote_key[64];
	uint8_t my_public_key[64];
	esp_aes_context aes_key;

	int asymmetric_ecdh_comm(uint8_t *remote_key, uint8_t *ecdh_value);
};


#endif
