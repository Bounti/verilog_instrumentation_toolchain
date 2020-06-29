/*
 * sha256.h
 *
 *  Created on: May 27, 2019
 *      Author: nasm
 */

#ifndef SRC_SHA256_H_
#define SRC_SHA256_H_

#include <stdlib.h>
#include <stdint.h>

#define ADDR_STATUS      0
#define ADDR_CTRL        1
#define ADDR_VERSION     2
#define ADDR_NAME0       3
#define ADDR_NAME1       4

#define CTRL_INIT_VALUE  0x01
#define CTRL_NEXT_VALUE  0x02
#define CTRL_MODE_VALUE  0x04

#define STATUS_READY_BIT 0
#define STATUS_VALID_BIT 1

#define ADDR_BLOCK0    5
#define ADDR_BLOCK1    6
#define ADDR_BLOCK2    7
#define ADDR_BLOCK3    8
#define ADDR_BLOCK4    9
#define ADDR_BLOCK5    10
#define ADDR_BLOCK6    11
#define ADDR_BLOCK7    12
#define ADDR_BLOCK8    13
#define ADDR_BLOCK9    14
#define ADDR_BLOCK10   15
#define ADDR_BLOCK11   16
#define ADDR_BLOCK12   17
#define ADDR_BLOCK13   18
#define ADDR_BLOCK14   19
#define ADDR_BLOCK15   20

#define ADDR_DIGEST0   21
#define ADDR_DIGEST1   22
#define ADDR_DIGEST2   23
#define ADDR_DIGEST3   24
#define ADDR_DIGEST4   25
#define ADDR_DIGEST5   26
#define ADDR_DIGEST6   27
#define ADDR_DIGEST7   28

#define SHA224_MODE    0
#define SHA256_MODE    1

#define SHA256_DIGEST_SIZE 32

typedef struct sha256_t
{
  uint32_t count;
  unsigned char buffer[64];
  uint32_t block_index;
} sha256_t;

void sha256_update(sha256_t *p, const unsigned char *data, size_t size);
void sha256_final(sha256_t *p, unsigned char *digest);
void sha256_hash(unsigned int *buf, const unsigned char *data, size_t size);
void sha256_write_byte_block(sha256_t *p);

#endif /* SRC_SHA256_H_ */
