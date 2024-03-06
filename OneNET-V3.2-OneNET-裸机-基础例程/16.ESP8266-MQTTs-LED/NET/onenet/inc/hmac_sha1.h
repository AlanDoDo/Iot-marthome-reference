#ifndef _HAMC_SHA1_H_
#define _HAMC_SHA1_H_





//
#define MAX_MESSAGE_LENGTH		4096


void hmac_sha1(
	unsigned char *key,
	int key_length,
	unsigned char *data,
	int data_length,
	unsigned char *digest
);


#endif
