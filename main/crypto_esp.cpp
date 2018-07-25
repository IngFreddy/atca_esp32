
#include "crypto_esp.h"


static const char* LOG_TAG = "Encryption";


/*
 * CryptoAuthLib Basics Disposable Asymmetric ECDH
 *
 * remote_key - Public remote key for secret [64b]
 *
 */
Encryption::Encryption()
{
	volatile ATCA_STATUS status;
	status = atcab_init((ATCAIfaceCfg*)&cfg_ateccx08a_i2c);			//REMOTE
	CHECK_STATUS(status);

	status = atcab_get_pubkey(ASYMETRIC_KEY_SLOT, my_public_key);	//read public keys
	CHECK_STATUS(status);
}


Encryption::~Encryption()
{
	atcab_release();
}


int Encryption::setRemote(uint8_t* pubKey)
{
	memcpy(remote_key, pubKey, 64);

	uint8_t ecdh_value[32];
	int ret = asymmetric_ecdh_comm(pubKey, ecdh_value); //ECDH secret key
	if(ret != 0){
		ESP_LOGE(LOG_TAG, "Error on ECDH secret code");
	}

	ESP_LOGI(LOG_TAG,"ECDH Value:\r\n");
	PRINT_BYTES(ecdh_value, 32);
	printf("\r\n");

	esp_aes_init(&aes_key);				//AES-128 ECB for encrypt data
	esp_aes_setkey(&aes_key, ecdh_value, 256);

	return ret;
}

/*
 * CryptoAuthLib Basics Disposable Asymmetric ECDH
 *
 * ecdh_value - output of Secret crypto key  [32b]
 *
 * return success/fail
 */
int Encryption::asymmetric_ecdh_comm(uint8_t *remote_key, uint8_t *ecdh_value)
{
	volatile ATCA_STATUS status;
	/*status = atcab_init((ATCAIfaceCfg*)&cfg_ateccx08a_i2c);			//REMOTE
	CHECK_STATUS(status);*/

	status = atcab_ecdh_enc(ASYMETRIC_KEY_SLOT, (const uint8_t*)remote_key, ecdh_value, transport_key, TRANSPORT_KEY_SLOT);
	CHECK_STATUS(status);

	//atcab_release();

	return 0;
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
int Encryption::encryptData(uint8_t* input, uint8_t* output, size_t length)
{
	uint8_t nonce[16];
	uint8_t stream[16];

	bzero(nonce, 16);
	bzero(stream, 16);

	size_t nc_off = 0;

	int ret=0;

	ret = esp_aes_crypt_ctr(&aes_key, length, &nc_off, nonce, stream, input, output);

	//esp_aes_crypt_ecb(&aes_key, ESP_AES_ENCRYPT, input, output);

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
int Encryption::decryptData( uint8_t* input, uint8_t* output, size_t length)
{
	uint8_t nonce[16];
	uint8_t stream[16];

	bzero(nonce, 16);
	bzero(stream, 16);

	size_t nc_off = 0;

	int ret=0;

	ret = esp_aes_crypt_ctr(&aes_key, length, &nc_off, nonce, stream, input, output);

	//esp_aes_crypt_ecb(&aes_key, ESP_AES_DECRYPT, input, output);

	return ret;
}

uint8_t* Encryption::getPublicKey(){
	return my_public_key;
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
