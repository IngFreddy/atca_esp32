/*
 * ATCA ATECC508A for ESP32
 * configure.h
 *
 * Created: 3. 7. 2018 13:46:20
 *  Author: Milan Mucka
 */


#include "provision.h"
#include "crypto_esp.h"


int write_config()
{
	volatile ATCA_STATUS status;
	status = atcab_init((ATCAIfaceCfg*)&cfg_ateccx08a_i2c);			//REMOTE
	CHECK_STATUS(status);

	bool islocked;
	status = atcab_is_locked(LOCK_ZONE_CONFIG, &islocked);
	CHECK_STATUS(status);

	if(islocked){
		printf("Config is locked!\n");
		return status;
	};
	
	status = atcab_write_config_zone(g_ecc508_config);
	CHECK_STATUS(status);
	
	status = atcab_lock_config_zone();
	CHECK_STATUS(status);
	
	printf("CONFIG Complete\r\n");


	return ATCA_SUCCESS;
}

int write_keys()
{
	const uint8_t key0[] = {
		
		0xf0, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x0f,
		
	};

	const uint8_t key1[] = {
		
		0xf1, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x1f,
		
	};

	const uint8_t key2[] = {
		
		0xf2, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x2f,
		
	};
	
	const uint8_t key3[] = {
		
		0xf3, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x11,
		0x11, 0x11, 0x11, 0x3f,
		
	};	
	
	ATCA_STATUS status;
	bool islocked;

	status = atcab_is_locked(LOCK_ZONE_DATA, &islocked);
	CHECK_STATUS(status);

	if(islocked){
		printf("SLOT 0 locked!\n");
	}

	printf("--Write Data 0--\r\n");
	status = atcab_write_bytes_zone(ATCA_ZONE_DATA, 0, 0, key0, 32);
	//status = atcab_read_bytes_zone(ATCA_ZONE_DATA, 0, 0, key0, 32);
	CHECK_STATUS(status);
	PRINT_BYTES(key0, 32);

	printf("--Write Data 1--\r\n");
	status = atcab_write_bytes_zone(ATCA_ZONE_DATA, 1, 0, key1, 32);
	CHECK_STATUS(status);

	printf("--Write Data 2--\r\n");
	status = atcab_write_bytes_zone(ATCA_ZONE_DATA, 2, 0, key2, 32);
	CHECK_STATUS(status);

	printf("--Write Data 3--\r\n");
	status = atcab_write_bytes_zone(ATCA_ZONE_DATA, 3, 0, key3, 32);
	CHECK_STATUS(status);
	
	printf("--Write Data 4--\r\n");
	uint8_t pubkey[64];
	status = atcab_genkey(4, pubkey);
	CHECK_STATUS(status);
	

	printf("Write Complete\r\n");
	printf("Locking Data Zone\r\n");
	status = atcab_lock_data_zone();
	CHECK_STATUS(status);

	return status;
}
