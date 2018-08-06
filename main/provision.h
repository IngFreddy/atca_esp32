/*
 * ATCA ATECC508A for ESP32
 * configure.h
 *
 * Created: 3. 7. 2018 13:46:20
 *  Author: Milan Mucka
 */ 


#ifndef PROVISION_H_
#define PROVISION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#include "cryptoauthlib.h"

/*
 * Write configuration data for ESP32 as generated from CryptoAuthBasic
 */
extern int write_config();

/*
 * Write keys data for ESP32
 */
extern int write_keys();



#endif
