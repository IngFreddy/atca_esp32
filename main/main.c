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
	0xe9, 0x75, 0x93, 0x50, 0x8b, 0xb0, 0x28, 0x2c,
	0xf2, 0x37, 0x20, 0xb3, 0x5e, 0x21, 0x2a, 0x49, 
	0xee, 0x37, 0x76, 0xee, 0xf2, 0x00, 0xe2, 0x2c, 
	0x09, 0xb2, 0x1b, 0xec, 0xdd, 0x3b, 0xfe, 0x6c, 
	0x75, 0x7d, 0x4f, 0x13, 0xe6, 0xf4, 0xc0, 0x08, 
	0xf2, 0x07, 0x87, 0x9e, 0xaf, 0x75, 0x4a, 0xad, 
	0x42, 0xe0, 0x2b, 0x64, 0xa8, 0x20, 0x64, 0x6b, 
	0xfe, 0xf0, 0x5b, 0xa1, 0xd7, 0x1e, 0xd6, 0xe8
};

bool authentificate(uint8_t* remoteKey)
{
	int status;
	uint8_t nonce[32];
	uint8_t signature[64];
	
	status = generate_random(nonce);
	CHECK_STATUS(status);
	printf("\nRandom from host\r\n");
	PRINT_BYTES(nonce, 32);

	status = signature_generate(nonce, signature);
	printf("\nSignature from remote\r\n");
	PRINT_BYTES(signature, 64);

	status = signature_verify(nonce, signature, remoteKey);
	if(status == 0){
		printf("\nAuthentificated succesfull\n\n");
	}
	else{
		printf("\nAuthentification failed\n\n");
	}

	return(status==0);
}

int app_main(void)
{
	//configure blank ECC508 boards
	//write_config();
	//write_keys();

	int status;
	uint8_t encrypted[20];
	uint8_t decrypted[20];
	uint8_t checksum[32];
	uint8_t signature[64];

	//SAMPLE TEXT
	printf("\n");
	ESP_LOGI(LOG_TAG,"START TEXT:\r\n");
	printf("%s\r\n", text);
	PRINT_BYTES(text, 16);
	printf("\n");

	//INIT OF ATECC defined in crypto_esp.h
	status = init_atca_device();
	CHECK_STATUS(status);

	//get public key
	uint8_t* myPublicKey = getPublicKey();
	printf("\nMy Public Key:\n");
	PRINT_BYTES(myPublicKey, 64);	
	printf("\n");

	//Authentificate remote with signature
	authentificate(remote_key);

	//set encryption environment, generate ECDH Secret
	setRemote(remote_key);

	//generate checksum for data
	checksum_data(text, 16, checksum);
	printf("\nChecksum of text:\n");
	PRINT_BYTES(checksum, 32);

	//Sign checksum for verification
	signature_generate(checksum, signature);
	printf("Signature of checksum:\n");
	PRINT_BYTES(signature, 64);
	
	//Encryption
	encryptData((uint8_t*)text, encrypted, 16);
	printf("\n");
	ESP_LOGI(LOG_TAG,"ENCRYPTED TEXT:\n");
	PRINT_BYTES(encrypted, 16);
	printf("\r\n");

	//Decryption
	decryptData(encrypted, decrypted, 16);
	ESP_LOGI(LOG_TAG,"DECRYPTED TEXT:\n");
	PRINT_BYTES(decrypted, 16);
	decrypted[16] = 0;
	printf("%s\r\n", decrypted);

	//generate checksum for data
	checksum_data(decrypted, 16, checksum);
	printf("\nChecksum of decrypted text:\n");
	PRINT_BYTES(checksum, 32);

	//verificate signature from checksum
	if(signature_verify(checksum, signature, remote_key) == 0){
		printf("\nSignature OK\n");
	}
	else{
		printf("\nSignature Error\n");
	}

	//release END
	release_atca_device();

	printf("\nEND\n");
	return 0;
}
