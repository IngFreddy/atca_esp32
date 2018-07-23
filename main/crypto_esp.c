
#include "crypto_esp.h"

static const char* LOG_TAG = "Cryptograpghy";



/*
 * CryptoAuthLib Basics Disposable Asymmetric ECDH
 *
 * remote_key - Public remote key for secret [64b]
 * ecdh_value - output of Secret crypto key  [32b]
 *
 * return success/fail
 */
void asymmetric_ecdh_comm(uint8_t *remote_key, uint8_t *ecdh_value)
{

	volatile ATCA_STATUS status;
	status = atcab_init((ATCAIfaceCfg*)&cfg_ateccx08a_i2c);			//REMOTE
	CHECK_STATUS(status);

	/*uint8_t serial_number[ATCA_SERIAL_NUM_SIZE];
	status = atcab_read_serial_number((uint8_t*)&serial_number);
	CHECK_STATUS(status);

	ESP_LOGI(LOG_TAG,"Serial Number of host\r\n");
	PRINT_BYTES(serial_number, 9);
	printf("\r\n");*/

	uint8_t my_pubk[64];
	uint8_t slot = 4;

	//read public keys
	status = atcab_get_pubkey(slot, my_pubk);
	CHECK_STATUS(status);
	ESP_LOGI(LOG_TAG,"MY public key\r\n");
	PRINT_BYTES(my_pubk, 64);

	ESP_LOGI(LOG_TAG,"REMOTE public key\r\n");
	//PRINT_BYTES((uint8_t*)remote_key, 64);

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

	uint8_t transport_key_slot = 2;

	//calculate ECDH value
	//uint8_t ecdh_value[32];

	status = atcab_ecdh_enc(slot, (const uint8_t*)remote_key, ecdh_value, transport_key, transport_key_slot);
	//status = atcab_ecdh(private_key_slot, (const uint8_t*)&remote_pubk, ecdh_value);
	CHECK_STATUS(status);

	//return status;
}

/*
 * Data encryption using ECDH function, SHA-256 hash and AES symetric cryptography
 *
 * key - master secret key for encryption [128b]
 * input - input data
 * output - encrypted data
 * length - size of data
 *
 *	return success/fail
 */
int encryptData(uint8_t *key, uint8_t* input, uint8_t* output, size_t length)
{
	uint8_t ecdh_value[32];
	asymmetric_ecdh_comm(key, ecdh_value); //ECDH secret key

	ESP_LOGI(LOG_TAG,"ECDH Value:\r\n");
	PRINT_BYTES(ecdh_value, 32);
	printf("\r\n");

	uint8_t secret_key[128];
	esp_sha(SHA2_512, ecdh_value, 32, secret_key);	//SHA-512 derivation of secret
	memcpy(secret_key+64, secret_key, 64);			//double for 128b

	ESP_LOGI(LOG_TAG,"SHA-512 Value:\r\n");
	PRINT_BYTES(secret_key, 128);
	printf("\r\n");

	esp_aes_context aes_ecdh;
	esp_aes_init(&aes_ecdh);				//AES-128 ECB for encrypt data
	esp_aes_setkey(&aes_ecdh, secret_key, 128);
	esp_aes_crypt_ecb(&aes_ecdh, ESP_AES_ENCRYPT, input, output);

	ESP_LOGI(LOG_TAG,"ENCRYPTED TEXT:\r\n");
	PRINT_BYTES(output, 16);
	printf("\r\n");

	return 0;
}

/*
 * Data encryption using ECDH function, SHA-256 hash and AES symetric cryptography
 *
 * key - master secret key for encryption [128b]
 * input - input data
 * output - encrypted data
 * length - size of data
 *
 *	return success/fail
 */
int decryptData(uint8_t *key, uint8_t* input, uint8_t* output, size_t length)
{
	uint8_t ecdh_value[32];
	asymmetric_ecdh_comm(key, ecdh_value); //ECDH secret key

	ESP_LOGI(LOG_TAG,"ECDH Value:\r\n");
	PRINT_BYTES(ecdh_value, 32);
	printf("\r\n");

	uint8_t secret_key[128];
	esp_sha(SHA2_512, ecdh_value, 32, secret_key);	//SHA-512 derivation of secret
	memcpy(secret_key+64, secret_key, 64);			//double for 128b

	ESP_LOGI(LOG_TAG,"SHA-512 Value:\r\n");
	PRINT_BYTES(secret_key, 128);
	printf("\r\n");

	esp_aes_context aes_ecdh;
	esp_aes_init(&aes_ecdh);				//AES-128 ECB for encrypt data
	esp_aes_setkey(&aes_ecdh, secret_key, 128);
	esp_aes_crypt_ecb(&aes_ecdh, ESP_AES_ENCRYPT, input, output);

	ESP_LOGI(LOG_TAG,"ENCRYPTED TEXT:\r\n");
	PRINT_BYTES(output, 16);
	printf("\r\n");

	return 0;
}

/*
//CryptoAuthLib Basics Disposable Asymmetric Auth
void asymmetric_auth(void) {

	//Step 3.1
	printf("CryptoAuthLib Basics Disposable Asymmetric Auth\n\r");
	volatile ATCA_STATUS status;

	printf("Authentication in progress\n\r");

	status = atcab_init( &cfg_ateccx08a_i2c_host );			//HOST
	CHECK_STATUS(status);
	printf("Device init complete\n\r");

	//Step 3.3
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

	//Step 3.4
	uint8_t remote_pubk[64];
	//status = atcab_read_bytes_zone(ATCA_ZONE_DATA,5, 0, &temp_pubk, 64);
	status = atcab_get_pubkey(slot, remote_pubk);
	CHECK_STATUS(status);
	printf("Remote disposable public key\r\n");
	PRINT_BYTES(remote_pubk, 64);

	//Step 3.6
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
