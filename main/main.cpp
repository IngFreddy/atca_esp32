#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <esp_log.h>

#include "crypto_esp.h"
#include "provision.h"

const unsigned char text[] = "Hi, how are you? ";
static const char* LOG_TAG = "MAIN";

extern "C" {
	extern void asymmetric_ecdh_comm(uint8_t *remote_key, uint8_t *ecdh_value);
    int app_main(void);
}// extern "C"

int app_main(void) {
	//configure(); //configure blank ECC508 boards

	ESP_LOGI(LOG_TAG,"START TEXT:\r\n");
	PRINT_BYTES(text, 16);
	printf("\r\n");

	uint8_t ecdh_value[32];
	asymmetric_ecdh_comm((uint8_t*)key_store, ecdh_value);

	ESP_LOGI(LOG_TAG,"ECDH Value:\r\n");
	PRINT_BYTES(ecdh_value, 32);
	printf("\r\n");

	uint8_t secret_key[128];
	esp_sha(SHA2_512, ecdh_value, 32, secret_key);
	memcpy(secret_key+64, secret_key, 64);
	ESP_LOGI(LOG_TAG,"SHA-512 Value:\r\n");
	PRINT_BYTES(secret_key, 128);
	printf("\r\n");

	uint8_t encrypted[16];
	esp_aes_context aes_ecdh;
	esp_aes_init(&aes_ecdh);
	esp_aes_setkey(&aes_ecdh, secret_key, 128);
	esp_aes_crypt_ecb(&aes_ecdh, ESP_AES_ENCRYPT, text, encrypted);

	ESP_LOGI(LOG_TAG,"ENCRYPTED TEXT:\r\n");
	PRINT_BYTES(encrypted, 16);
	printf("\r\n");

	uint8_t output[16];
	esp_aes_crypt_ecb(&aes_ecdh, ESP_AES_DECRYPT, encrypted, output);


	ESP_LOGI(LOG_TAG,"DECRYPTED TEXT:\r\n");
	PRINT_BYTES(output, 16);
	printf("\r\n");
	printf("%s \r\n", text);

	printf("END\n");
	return 0;
}
