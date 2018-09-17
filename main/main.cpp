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

#include "crypto_esp.h"
#include "provision.h"

const unsigned char text[] = "Hi, how are you? ";
size_t text_length = 16;

static const char* LOG_TAG = "MAIN";

//Public key from other client
uint8_t remote_key[] = {
0x12, 0x22, 0x7b, 0x94, 0x78, 0x26, 0x59, 0x7f, 
0xd3, 0x23, 0xfa, 0x91, 0x38, 0x40, 0x62, 0x2c, 
0xa6, 0x5f, 0x7d, 0xca, 0xa6, 0x8e, 0x1d, 0x93, 
0x60, 0x44, 0x5f, 0xe9, 0x36, 0x22, 0x4c, 0x50, 
0xae, 0x0d, 0xa9, 0x50, 0xbc, 0xcd, 0xb5, 0x61, 
0x49, 0xab, 0x08, 0x3a, 0x1b, 0xc2, 0xe9, 0xb5, 
0xc7, 0xc6, 0x82, 0xf1, 0x20, 0xf2, 0x25, 0xa0, 
0x2c, 0x06, 0x59, 0xbf, 0xe6, 0x36, 0x75, 0xf9,
};

extern "C" {
	bool authentificate(Cryptography *, uint8_t*);
    int app_main(void);
}

bool authentificate(Cryptography *crypto, uint8_t* remoteKey)
{
	int status;
	uint8_t nonce[32];
	uint8_t signature[64];
	
	//generate nonce random for signature
	status = crypto->generate_random(nonce);
	CHECK_STATUS(status);
	printf("\nRandom from host:\n");
	PRINT_BYTES(nonce, 32);

	//sign the nonce 
	status = crypto->signature_generate(nonce, signature);
	printf("\nSignature from remote\n");
	PRINT_BYTES(signature, 64);

	//verify the signature of nonce with remote public key
	status = crypto->signature_verify(nonce, signature, remoteKey);
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

	Cryptography *crypto = new Cryptography();		//init ATCA device

	uint8_t encrypted[20];
	uint8_t decrypted[20];
	uint8_t checksum[32];
	uint8_t signature[64];

	//SAMPLE TEXT write out
	printf("\n");
	ESP_LOGI(LOG_TAG,"START TEXT: ");
	printf("%s\n", text);
	PRINT_BYTES(text, text_length);
	printf("\n");


	//get and write public key
	uint8_t* myPublicKey = crypto->public_key();
	printf("\nMy Public Key:\n");
	PRINT_BYTES(myPublicKey, 64);	
	printf("\n");

	//Authentificate remote with signature
	authentificate(crypto, myPublicKey);

	//set encryption environment, generate ECDH Secret
	crypto->set_remote(remote_key);

	//generate checksum for data
	crypto->checksum_data((uint8_t*)text, text_length, checksum);
	printf("\nChecksum of text:\n");
	PRINT_BYTES(checksum, 32);

	//Sign checksum for verification
	crypto->signature_generate(checksum, signature);
	printf("Signature of checksum:\n");
	PRINT_BYTES(signature, 64);
	
	//Encryption
	crypto->encryptData((uint8_t*)text, encrypted, text_length);
	printf("\n");
	ESP_LOGI(LOG_TAG,"ENCRYPTED TEXT:\n");
	PRINT_BYTES(encrypted, text_length);
	printf("\r\n");

	//Decryption
	crypto->decryptData(encrypted, decrypted, text_length);
	ESP_LOGI(LOG_TAG,"DECRYPTED TEXT:\n");
	PRINT_BYTES(decrypted, text_length);
	decrypted[text_length] = 0;
	printf("%s\r\n", decrypted);

	//generate checksum for data
	crypto->checksum_data(decrypted, text_length, checksum);
	printf("\nChecksum of decrypted text:\n");
	PRINT_BYTES(checksum, 32);

	//verificate signature and checksum
	if(crypto->signature_verify(checksum, signature, remote_key) == 0){
		printf("\nText Signature OK\n");
	}
	else{
		printf("\nText Signature Error\n");
	}

	//release END
	crypto->~Cryptography();

	printf("\nEND\n");
	return 0;
}
