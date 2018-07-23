/*
 * ATCA ATECC508A for ESP32
 * configure.h
 *
 * Created: 3. 7. 2018 13:46:20
 *  Author: Milan Mucka
 */ 


#ifndef PROVISION_H_
#define PROVISION_H_

/*
 * Write configuration data for ESP32 as generated from CryptoAuthBasic
 */
extern ATCA_STATUS write_config();

/*
 * Write keys data for ESP32
 */
extern ATCA_STATUS write_keys();


#define CHECK_STATUS(s)  if(s != ATCA_SUCCESS) { printf("Error: Line %d in %s\r\n", __LINE__, __FILE__); while(1);	}

#define PRINT_BYTES(ptr, length)				\
do{ uint8_t line_count = 0;						\
	for(uint8_t i = 0;i < length; i++) {		\
		printf("0x%02x, ",ptr[i]); line_count++;\
		if(line_count == 8) { printf("\n"); }}\
} while(0); printf("\r\n");


#endif
