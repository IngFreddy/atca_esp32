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

esp_aes_context aes_key;

static const char* LOG_TAG = "Encryption";


Cryptography::Cryptography()
{
	init_atca_device();
}

Cryptography::~Cryptography()
{
	this->release_atca_device();
}

/*
 * Initialize CryptoAuthLib Basics Disposable Asymmetric ECDH
 */
int Cryptography::init_atca_device()
{
	volatile ATCA_STATUS status;
	status = atcab_init((ATCAIfaceCfg*)&cfg_ateccx08a_i2c);			//REMOTE
	CHECK_STATUS(status);
	
	status = atcab_get_pubkey(ASYMETRIC_KEY_SLOT, my_public_key);	//read public keys

	return status;
}

/*
 * get ECDH Secret key from chip
 */
ATCA_STATUS asymmetric_ecdh_comm(uint8_t *remote_key, uint8_t *ecdh_value)
{
	volatile ATCA_STATUS status;

	status = atcab_ecdh_enc(ASYMETRIC_KEY_SLOT, (const uint8_t*)remote_key, ecdh_value, transport_key, TRANSPORT_KEY_SLOT);

	return status;
}

/*
 *	Set remote public key for encryption
*/
int Cryptography::set_remote(uint8_t* pubKey)
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
 * Return Public key of initalized chip
 */
uint8_t* Cryptography::public_key()
{
	return this->my_public_key;
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
int Cryptography::encryptData(uint8_t* input, uint8_t* output, size_t length)
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
int Cryptography::decryptData( uint8_t* input, uint8_t* output, size_t length)
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
 * Generate random for nonce from device ATCA
 * Need nonce[32] allocated
 * 
 * return state
 */
int Cryptography::generate_random(uint8_t* nonce)
{
	return atcab_random(nonce);
}

/*
 * Verification of signature
 * 
 * nonce[32]
 * sugnature[64]
 * remoteKey[64]
 * 
 * return 0 on success
*/
int Cryptography::signature_verify(uint8_t* nonce, uint8_t* signature, uint8_t* remoteKey)
{
	volatile ATCA_STATUS status;

	bool verify = false;

	status = atcab_verify_extern((const uint8_t*)nonce, signature, remoteKey, &verify);		//VERIFY SIGNATURE

	if(verify && status == ATCA_SUCCESS) {
		return 0;
	} else {
		return(-1);
	}
}

/*
 * Sign NONCE from host for verification
 * 
 * nonce[32]
 * signature[64]
 * 
 * return state
*/
int Cryptography::signature_generate(uint8_t* nonce, uint8_t* signature)
{
	volatile ATCA_STATUS status;

	status = atcab_sign(ASYMETRIC_KEY_SLOT, nonce, signature);

	return status;
}

/*
 * SHA-256 checksum of data
 * 
 * input data[length]
 * output [32]	--checksum of data
 * 
 * return status
 *
 */
int Cryptography::checksum_data(uint8_t *data, size_t length, uint8_t *output)
{
	esp_sha(SHA2_256, data, length, output);
	
	return length;
}

/* 
 * Release device on the end
*/
void Cryptography::release_atca_device()
{
	atcab_release();
}