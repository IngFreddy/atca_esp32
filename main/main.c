/*
 * ATCA ATECC508A Cryptography Demo for ESP32
 * AES-CTR Cryptography example on esp32 HW accelerated
 * 
 * Created: 3. 7. 2018 13:46:20
 * Author: Milan Mucka - milan.freddy@gmail.com
 * 
 * Code is published as it is, without any warranty for working state.
 * If you find some bugs, please create issue task on github.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "crypto_esp.h"
#include "provision.h"


const unsigned char text[] = "Hi, how are you? ";
static const char* LOG_TAG = "MAIN";

//Public key from other client
uint8_t remote_key[] = {
	0x33, 0xd8, 0x4a, 0xc4, 0x63, 0xf0, 0xf4, 0x30,
	0x9d, 0xf6, 0x3a, 0xe9, 0xea, 0x7e, 0x36, 0x23,
	0x1e, 0x30, 0xeb, 0x9c, 0x29, 0x73, 0xe4, 0xb9,
	0xcc, 0x29, 0x4f, 0x8a, 0x9b, 0x37, 0x2f, 0x02,
	0x9c, 0xb4, 0xc0, 0x4a, 0x12, 0x4f, 0xa9, 0xf3,
	0x68, 0xbf, 0xd7, 0xe2, 0xdb, 0x74, 0x17, 0x27,
	0x3d, 0x4d, 0xc0, 0x2a, 0x04, 0xb5, 0x6e, 0xec,
	0x8b, 0xec, 0xbb, 0xdc, 0xaf, 0x69, 0xa1, 0x0f
};


int app_main(void)
{
	//write_config(); //configure blank ECC508 boards
	//write_keys();

	int status;
	uint8_t encrypted[20];
	uint8_t decrypted[20];
	
	printf("\n");
	ESP_LOGI(LOG_TAG,"START TEXT:\r\n");
	printf("%s\r\n", text);
	PRINT_BYTES(text, 16);
	printf("\n");

	status = init_atca_device();
	CHECK_STATUS(status);

	printf("\nMy Public Key:\n");
	PRINT_BYTES(my_public_key, 64);	
	printf("\n");

	setRemote(remote_key);
	
	encryptData((uint8_t*)text, encrypted, 16);
	
	printf("\n");
	ESP_LOGI(LOG_TAG,"ENCRYPTED TEXT:\n");
	PRINT_BYTES(encrypted, 16);
	printf("\r\n");

	decryptData(encrypted, decrypted, 16);

	ESP_LOGI(LOG_TAG,"DECRYPTED TEXT:\n");
	PRINT_BYTES(decrypted, 16);
	printf("%s\r\n", decrypted);

	release_atca_device();

	printf("\nEND\n");
	return 0;
}
