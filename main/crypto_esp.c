/*
 * ATCA ATECC508A for ESP32
 * crypto_esp.h
 * 
 * Cryptographic library for ESP32
 * AEC-CTR
 *
 * Created: 3. 7. 2018 13:46:20
 *  Author: Milan Mucka
 */

#include "crypto_esp.h"

uint8_t* my_public_key = NULL;

static const char* LOG_TAG = "Encryption";

/*
 * Initialize CryptoAuthLib Basics Disposable Asymmetric ECDH
 */
int init_atca_device()
{
	volatile ATCA_STATUS status;
	status = atcab_init((ATCAIfaceCfg*)&cfg_ateccx08a_i2c);			//REMOTE
	CHECK_STATUS(status);
	
	my_public_key = (uint8_t*)malloc(64);
	if(my_public_key == NULL){
		printf("error Malloc init public key\n");
		return -5;
	}

	status = atcab_get_pubkey(ASYMETRIC_KEY_SLOT, my_public_key);	//read public keys

	return status;
}

int asymmetric_ecdh_comm(uint8_t *remote_key, uint8_t *ecdh_value)
{
	volatile ATCA_STATUS status;

	status = atcab_ecdh_enc(ASYMETRIC_KEY_SLOT, (const uint8_t*)remote_key, ecdh_value, transport_key, TRANSPORT_KEY_SLOT);

	return status;
}

/*
 *	Set remote public key for encryption
*/
int setRemote(uint8_t* pubKey)
{
	uint8_t ecdh_value[32];
	ATCA_STATUS status = asymmetric_ecdh_comm(pubKey, ecdh_value); //ECDH secret key
	if(status != ATCA_SUCCESS){
		ESP_LOGE(LOG_TAG, "Error on ECDH secret code");
		return status;
	}

	esp_aes_init(&aes_key);				//AES-128 ECB for encrypt data
	esp_aes_setkey(&aes_key, ecdh_value, 256);

	return status;
}

/*
 * Data encryption using ECDH function, SHA-256 hash and AES symetric cryptography
 *
 * input - input data
 * output - encrypted data
 * length - size of data
 *
 *	return success/fail
 */
int encryptData(uint8_t* input, uint8_t* output, size_t length)
{
	uint8_t nonce[16];
	uint8_t stream[16];

	bzero(nonce, 16);
	bzero(stream, 16);

	size_t nc_off = 0;

	int ret=0;

	ret = esp_aes_crypt_ctr(&aes_key, length, &nc_off, nonce, stream, input, output);

	return ret;
}

/*
 * Data encryption using ECDH function, SHA-256 hash and AES symetric cryptography
 *
 * input - input data
 * output - encrypted data
 * length - size of data
 *
 *	return success/fail
 */
int decryptData( uint8_t* input, uint8_t* output, size_t length)
{
	uint8_t nonce[16];
	uint8_t stream[16];

	bzero(nonce, 16);
	bzero(stream, 16);

	size_t nc_off = 0;

	int ret=0;

	ret = esp_aes_crypt_ctr(&aes_key, length, &nc_off, nonce, stream, input, output);

	return ret;
}


void release_atca_device()
{
	atcab_release();
	free(my_public_key);
}


/* AUTHENTIFICATION
//CryptoAuthLib Basics Disposable Asymmetric Auth
void asymmetric_auth(void) {
	volatile ATCA_STATUS status;

	status = atcab_init( &cfg_ateccx08a_i2c_host );			//HOST
	CHECK_STATUS(status);
	printf("Device init complete\n\r");

	uint8_t nonce[32];
	uint8_t signature[64];

	status = atcab_random((uint8_t*)&nonce);
	CHECK_STATUS(status);
	printf("Random from host\r\n");
	PRINT_BYTES(nonce, 32);

	status = atcab_init( &cfg_ateccx08a_i2c_remote );		//REMOTE
	uint8_t slot = 4;

	status = atcab_sign(slot, (const uint8_t*)&nonce, (uint8_t*)&signature);
	CHECK_STATUS(status);
	printf("Signature from remote\r\n");
	PRINT_BYTES(signature, 64);

	uint8_t remote_pubk[64];
	status = atcab_get_pubkey(slot, remote_pubk);
	CHECK_STATUS(status);
	printf("Remote disposable public key\r\n");
	PRINT_BYTES(remote_pubk, 64);

	status = atcab_init( &cfg_ateccx08a_i2c_host );			//HOST
	CHECK_STATUS(status);

	bool verify = false;
	bool key_found = false;

	uint8_t i = 0;
	for(;i < sizeof(key_store)/sizeof(asymm_public_key_t); i++) {	//FIND PUBLIC KEY
		if(memcmp(&key_store[i], &remote_pubk, 64) == 0) {
			key_found = true;
			break;
		}
	}
	if(key_found) {
		status = atcab_verify_extern((const uint8_t*)&nonce,(const uint8_t*)&signature,(const uint8_t*)&remote_pubk, &verify);		//VERIFY SIGNATURE
	}
	if(verify) {
		printf("Authenticated by host\r\n");
	} else {
		printf("Failed to authenticate\r\n");
	}
}*/
