/*
 * aes_crt.h
 *
 *  Created on: Jun 5, 2019
 *      Author: nasm
 */

#ifndef SRC_AES_CRT_H_
#define SRC_AES_CRT_H_

#define BITS_PER_BYTE   8
#define BYTES_PER_WORD   4
#define BITS_PER_WORD (BITS_PER_BYTE * BYTES_PER_WORD)

#define AES_BASE   0x43c00000

// AES peripheral
#define AES_NO_KEYS 3
#define AES_PT_BITS   128
#define AES_ST_BITS   128
#define AES_KEY_BITS   192
#define AES_CT_BITS   128
#define AES_START_WORDS  1
#define AES_DONE_WORDS   1
#define AES_PT_WORDS (AES_PT_BITS / BITS_PER_WORD)
#define AES_ST_WORDS (AES_ST_BITS / BITS_PER_WORD)
#define AES_KEY_WORDS (AES_KEY_BITS / BITS_PER_WORD)
#define AES_CT_WORDS (AES_CT_BITS / BITS_PER_WORD)

#define AES_START   AES_BASE
#define AES_PT_BASE   ( AES_START     + 4*AES_START_WORDS )
#define AES_KEY0_BASE ( AES_PT_BASE   + 4*AES_PT_WORDS )
#define AES_DONE      ( AES_KEY0_BASE + 4*AES_KEY_WORDS )
#define AES_CT_BASE   ( AES_DONE      + 4*AES_DONE_WORDS )
#define AES_ST_BASE   ( AES_CT_BASE   + 4*AES_CT_WORDS )
#define AES_KEY1_BASE ( AES_ST_BASE   + 4*AES_ST_WORDS )
#define AES_KEY2_BASE ( AES_KEY1_BASE + 4*AES_KEY_WORDS )
#define AES_KEY_SEL   ( AES_KEY2_BASE + 4*AES_KEY_WORDS )

#define AES_START_DELAY 1
#define AES_DONE_DELAY 4

// declare the functions
uint32_t readFromAddress(uint32_t pAddress);
void writeToAddress(uint32_t pAddress, uint32_t pData);
void readMultiFromAddress(uint32_t pAddress, uint32_t *pData, int block_size);
void writeMultiToAddress(uint32_t pAddress, uint32_t *pData, int block_size);
void writeMulticharToAddress(uint32_t pAddress, char *pData, int block_byte_size);

void my_delay(int wait_cycles);
int verifyMulti(uint32_t *expectedData, uint32_t *receivedData, int block_size);

void aes_encrypt(uint32_t *pt, uint32_t *st, uint32_t *ct, uint32_t key_sel);

int check_aes();

#endif /* SRC_AES_CRT_H_ */
