/*
 * aes.c
 *
 *  Created on: May 29, 2019
 *      Author: nasm
 */

#include "aes.h"

#include "xil_printf.h"

// volatile unsigned int* aes_reg = (volatile unsigned int*) 0x43c10000;
volatile unsigned int *aes_reg = (volatile unsigned int *)0x43c30000;

//----------------------------------------------------------------
// aes_wait_ready()
//
// Wait for the ready flag in the dut to be set.
// (Actually we wait for either ready or valid to be set.)
//
// Note: It is the callers responsibility to call the function
// when the dut is actively processing and will in fact at some
// point set the flag.
//----------------------------------------------------------------
void aes_wait_ready() {
  unsigned int status = 1;
  do {
    status = aes_reg[ADDR_AES_STATUS] & 1;
    if (status == 0) {
      printf("Waiting crypto block to complete...");
    }
  } while (status == 0);
}

//----------------------------------------------------------------
// aes_wait_valid()
//
// Wait for the valid flag in the dut to be set.
// (Actually we wait for either ready or valid to be set.)
//
// Note: It is the callers responsibility to call the function
// when the dut is actively processing and will in fact at some
// point set the flag.
//----------------------------------------------------------------
void aes_wait_valid() {
  unsigned int status = 1;
  do {
    status = (aes_reg[ADDR_AES_STATUS] >> 1) & 1;
    if (status == 0) {
      printf("Waiting crypto block to complete...");
    }
  } while (status == 0);
}

//----------------------------------------------------------------
// init_key()
//
// init the key in the dut by writing the given key and
// key length and then trigger init processing.
//----------------------------------------------------------------
void init_key(unsigned int key[8], unsigned int key_length) {

  //	aes_reg[ADDR_AES_KEY0] = key[7];
  //	aes_reg[ADDR_AES_KEY1] = key[6];
  //	aes_reg[ADDR_AES_KEY2] = key[5];
  //	aes_reg[ADDR_AES_KEY3] = key[4];
  //	aes_reg[ADDR_AES_KEY4] = key[3];
  //	aes_reg[ADDR_AES_KEY5] = key[2];
  //	aes_reg[ADDR_AES_KEY6] = key[1];
  //	aes_reg[ADDR_AES_KEY7] = key[0];

  //  	aes_reg[ADDR_AES_CTRL] = 0x1;
  //  	aes_reg[ADDR_AES_CTRL] = 0x0;
  //

  // Need a status register here ?
  // #(100 * CLK_PERIOD);
} // init_key

//----------------------------------------------------------------
// ecb_mode_single_block_test()
//
// Perform ECB mode encryption or decryption single block test.
//----------------------------------------------------------------
void ecb_mode_single_block_test(unsigned char encdec, unsigned int key[8],
                                unsigned int key_length, unsigned int block[4],
                                unsigned int *result) {
  aes_wait_ready();

  // SET KEY
  //	#1 axi_write(32'd24,32'h2b7e1516);
  //	#1 axi_write(32'd28,32'h28aed2a6);
  //	#1 axi_write(32'd32,32'habf71588);
  //	#1 axi_write(32'd36,32'h09cf4f3c);
  //	#1 axi_write(32'd40,32'h00000000);
  //	#1 axi_write(32'd44,32'h00000000);
  //	#1 axi_write(32'd48,32'h00000000);
  //	#1 axi_write(32'd52,32'h00000000);
  aes_reg[ADDR_AES_KEY0] = key[0];
  aes_reg[ADDR_AES_KEY1] = key[1];
  aes_reg[ADDR_AES_KEY2] = key[2];
  aes_reg[ADDR_AES_KEY3] = key[3];
  aes_reg[ADDR_AES_KEY4] = key[4];
  aes_reg[ADDR_AES_KEY5] = key[5];
  aes_reg[ADDR_AES_KEY6] = key[6];
  aes_reg[ADDR_AES_KEY7] = key[7];

  // SET CONFIG
  //		#1 axi_write(32'd4,32'h00000001);
  // aes_reg[ADDR_AES_CONFIG] = (key_length << 1)+ encdec;
  aes_reg[ADDR_AES_CONFIG] = 1;

  // SET CONTROL -> INIT
  //	#1 axi_write(32'd0,32'h00000001);
  //	#1 axi_write(32'd0,32'h00000000);
  aes_reg[ADDR_AES_CTRL] = 1;
  aes_reg[ADDR_AES_CTRL] = 0;

  aes_wait_ready();

  //#1 axi_write(32'd56,32'h6bc1bee2);
  //#1 axi_write(32'd60,32'h2e409f96);
  //#1 axi_write(32'd64,32'he93d7e11);
  //#1 axi_write(32'd68,32'h7393172a);
  aes_reg[ADDR_AES_BLOCK0] = block[0];
  aes_reg[ADDR_AES_BLOCK1] = block[1];
  aes_reg[ADDR_AES_BLOCK2] = block[2];
  aes_reg[ADDR_AES_BLOCK3] = block[3];

  // SET CONTROL -> NEXT
  //	#1 axi_write(32'd0,32'h00000002);
  //	#1 axi_write(32'd0,32'h00000000);
  aes_reg[ADDR_AES_CTRL] = 2;
  aes_reg[ADDR_AES_CTRL] = 0;

  aes_wait_valid();

  result[0] = aes_reg[ADDR_AES_RESULT0];
  result[1] = aes_reg[ADDR_AES_RESULT1];
  result[2] = aes_reg[ADDR_AES_RESULT2];
  result[3] = aes_reg[ADDR_AES_RESULT3];

} // ecb_mode_single_block_test
