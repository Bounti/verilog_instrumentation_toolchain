/*
 * sha256.c
 *
 *  Created on: May 27, 2019
 *      Author: nasm
 */

/* Crypto/Sha256.c -- SHA-256 Hash
2010-06-11 : Igor Pavlov : Public domain
This code is based on public domain code from Wei Dai's Crypto++ library. */

//#include "rotate-bits/rotate-bits.h"
#include "sha256.h"

/* define it for speed optimization */
#define _SHA256_UNROLL
#define _SHA256_UNROLL2

volatile unsigned int *sha256_reg = (volatile unsigned int *)0x60000000;

//----------------------------------------------------------------
// wait_ready()
//
// Wait for the ready flag in the dut to be set.
// (Actually we wait for either ready or valid to be set.)
//
// Note: It is the callers responsibility to call the function
// when the dut is actively processing and will in fact at some
// point set the flag.
//----------------------------------------------------------------
void wait_ready() {
  unsigned int status = 1;
  do {
    status = sha256_reg[ADDR_STATUS];
  } while (status == 0);
}

//----------------------------------------------------------------
// write_block()
//
// Write the given block to the dut.
//----------------------------------------------------------------
void write_block(uint32_t block[]) {
  sha256_reg[ADDR_BLOCK0] = block[0];
  sha256_reg[ADDR_BLOCK1] = block[1];
  sha256_reg[ADDR_BLOCK2] = block[2];
  sha256_reg[ADDR_BLOCK3] = block[3];
  sha256_reg[ADDR_BLOCK4] = block[4];
  sha256_reg[ADDR_BLOCK5] = block[5];
  sha256_reg[ADDR_BLOCK6] = block[6];
  sha256_reg[ADDR_BLOCK7] = block[7];
  sha256_reg[ADDR_BLOCK8] = block[8];
  sha256_reg[ADDR_BLOCK9] = block[9];
  sha256_reg[ADDR_BLOCK10] = block[10];
  sha256_reg[ADDR_BLOCK11] = block[11];
  sha256_reg[ADDR_BLOCK12] = block[12];
  sha256_reg[ADDR_BLOCK13] = block[13];
  sha256_reg[ADDR_BLOCK14] = block[14];
  sha256_reg[ADDR_BLOCK15] = block[15];
}

//----------------------------------------------------------------
// read_digest()
//
// Read the digest in the dut. The resulting digest will be
// available in the global variable digest_data.
//----------------------------------------------------------------
void read_digest(unsigned int *digest) {

  //	int i;
  //
  //	for (i = 7; i >= 0; i--)
  //	{
  //		unsigned tmp = sha256_reg[ADDR_DIGEST0+i];
  //
  //		*digest++ = (unsigned char)(tmp >> 24 & 0xFF);
  //		*digest++ = (unsigned char)(tmp >> 16 & 0xFF);
  //		*digest++ = (unsigned char)(tmp >> 8  & 0xFF);
  //		*digest++ = (unsigned char)(tmp & 0xFF);
  //	}

  digest[0] = sha256_reg[ADDR_DIGEST7];
  digest[1] = sha256_reg[ADDR_DIGEST6];
  digest[2] = sha256_reg[ADDR_DIGEST5];
  digest[3] = sha256_reg[ADDR_DIGEST4];
  digest[4] = sha256_reg[ADDR_DIGEST3];
  digest[5] = sha256_reg[ADDR_DIGEST2];
  digest[6] = sha256_reg[ADDR_DIGEST1];
  digest[7] = sha256_reg[ADDR_DIGEST0];
}

void sha256_hash(unsigned int *buf, const unsigned char *data, size_t size) {
  sha256_t hash;

  // init
  hash.count = 0;
  hash.block_index = 0;

  // compute blocks
  sha256_update(&hash, data, size);

  sha256_final(&hash, buf);

  read_digest(buf);
}

void sha256_update(sha256_t *p, const unsigned char *data, size_t size) {
  uint32_t curBufferPos = (uint32_t)p->count & 0x3F;
  while (size > 0) {
    p->buffer[curBufferPos++] = *data++;
    p->count++;
    size--;
    if (curBufferPos == 64) {
      curBufferPos = 0;
      sha256_write_byte_block(p);
    }
  }
}

void sha256_write_byte_block(sha256_t *p) {
  uint32_t data32[16];
  unsigned i;
  for (i = 0; i < 16; i++)
    data32[i] = ((uint32_t)(p->buffer[i * 4]) << 24) +
                ((uint32_t)(p->buffer[i * 4 + 1]) << 16) +
                ((uint32_t)(p->buffer[i * 4 + 2]) << 8) +
                ((uint32_t)(p->buffer[i * 4 + 3]));

  write_block(data32);

  if (p->block_index != 0) {
    sha256_reg[ADDR_CTRL] = (CTRL_MODE_VALUE + CTRL_NEXT_VALUE);
    sha256_reg[ADDR_CTRL] = (CTRL_MODE_VALUE);
  } else {
    sha256_reg[ADDR_CTRL] = (CTRL_MODE_VALUE + CTRL_INIT_VALUE);
    sha256_reg[ADDR_CTRL] = (CTRL_MODE_VALUE);
  }

  p->block_index++;

  wait_ready();
}

void sha256_final(sha256_t *p, unsigned char *digest) {
  uint64_t lenInBits = (p->count << 3);
  uint32_t curBufferPos = (uint32_t)p->count & 0x3F;
  unsigned i;
  p->buffer[curBufferPos++] = 0x80;
  while (curBufferPos != (64 - 8)) {
    curBufferPos &= 0x3F;
    if (curBufferPos == 0)
      sha256_write_byte_block(p);
    p->buffer[curBufferPos++] = 0;
  }
  for (i = 0; i < 8; i++) {
    p->buffer[curBufferPos++] = (unsigned char)(lenInBits >> 56);
    lenInBits <<= 8;
  }
  sha256_write_byte_block(p);
}
