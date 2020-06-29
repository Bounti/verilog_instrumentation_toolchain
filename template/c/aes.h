/*
 * aes.h
 *
 *  Created on: May 29, 2019
 *      Author: nasm
 */

#ifndef SRC_AES_H_
#define SRC_AES_H_

// The DUT address map.
#define ADDR_AES_CTRL        0
#define ADDR_AES_CONFIG      1
#define ADDR_AES_STATUS      2
#define ADDR_AES_VERSION     3
#define ADDR_AES_NAME0       4
#define ADDR_AES_NAME1       5
#define ADDR_AES_KEY0        6
#define ADDR_AES_KEY1        7
#define ADDR_AES_KEY2        8
#define ADDR_AES_KEY3        9
#define ADDR_AES_KEY4        10
#define ADDR_AES_KEY5        11
#define ADDR_AES_KEY6        12
#define ADDR_AES_KEY7        13

#define ADDR_AES_BLOCK0      14
#define ADDR_AES_BLOCK1      15
#define ADDR_AES_BLOCK2      16
#define ADDR_AES_BLOCK3      17

#define ADDR_AES_RESULT0     18
#define ADDR_AES_RESULT1     19
#define ADDR_AES_RESULT2     20
#define ADDR_AES_RESULT3     21

#define CTRL_INIT_BIT    0x0
#define CTRL_NEXT_BIT    0x1
#define CTRL_ENCDEC_BIT  0x2
#define CTRL_KEYLEN_BIT  0x3

#define STATUS_READY_BIT 0x0
#define STATUS_VALID_BIT 0x1

#define AES_128_BIT_KEY 0
#define AES_256_BIT_KEY 1

#define AES_DECIPHER 0
#define AES_ENCIPHER 1

//----------------------------------------------------------------
// ecb_mode_single_block_test()
//
// Perform ECB mode encryption or decryption single block test.
//----------------------------------------------------------------
void ecb_mode_single_block_test(unsigned char   encdec,
								unsigned int    key[8],
								unsigned int    key_length,
								unsigned int    block[4],
								unsigned int    result[4]);

//----------------------------------------------------------------
// init_key()
//
// init the key in the dut by writing the given key and
// key length and then trigger init processing.
//----------------------------------------------------------------
void init_key(unsigned int key[8], unsigned int key_length);

#endif /* SRC_AES_H_ */
