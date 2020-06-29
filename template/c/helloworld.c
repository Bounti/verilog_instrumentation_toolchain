/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"

#include "sha256.h"
#include "aes.h"
#include "aes_crt.h"

void test_sha256(unsigned int test_id) {
    unsigned int buf[8] = {0};
    const unsigned char* test[4] = {
			(const unsigned char*)"abc",
			(const unsigned char*)"",
			(const unsigned char*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    		(const unsigned char*)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
    };

    unsigned int test_oracle[4][8] = {
			{0xba7816bf, 0x8f01cfea, 0x414140de, 0x5dae2223, 0xb00361a3, 0x96177a9c, 0xb410ff61, 0xf20015ad},
			{0xe3b0c442, 0x98fc1c14, 0x9afbf4c8, 0x996fb924, 0x27ae41e4, 0x649b934c, 0xa495991b, 0x7852b855},
			{0x248d6a61, 0xd20638b8, 0xe5c02693, 0x0c3e6039, 0xa33ce459, 0x64ff2167, 0xf6ecedd4, 0x19db06c1},
    		{0xcf5b16a7, 0x78af8380, 0x036ce59e, 0x7b049237, 0x0b249b11, 0xe8f07a51, 0xafac4503, 0x7afee9d1}
    };

	sha256_hash(buf, test[test_id], strlen((const char*)test[test_id]));

	if(memcmp(buf, test_oracle[test_id], 32) != 0) {
		printf("Error, sha256 test case %d failed", test_id);
		//error case
		while(1);
	}
}

void test_aes() {
    unsigned int nist_aes128_key[] = {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
//	unsigned int nist_aes256_key[] = {0x603deb10, 0x15ca71be, 0x2b73aef0, 0x857d7781, 0x1f352c07, 0x3b6108d7, 0x2d9810a3, 0x0914dff4};

	unsigned int nist_plaintext0[4] = {0x6bc1bee2, 0x2e409f96, 0xe93d7e11, 0x7393172a};
//	unsigned int nist_plaintext1[] = {0xae2d8a57, 0x1e03ac9c, 0x9eb76fac, 0x45af8e51};
//	unsigned int nist_plaintext2[] = {0x30c81c46, 0xa35ce411, 0xe5fbc119, 0x1a0a52ef};
//	unsigned int nist_plaintext3[] = {0xf69f2445, 0xdf4f9b17, 0xad2b417b, 0xe66c3710};

//	unsigned int nist_ecb_128_enc_expected0[] = {0x3ad77bb4, 0x0d7a3660, 0xa89ecaf3, 0x2466ef97};
//	unsigned int nist_ecb_128_enc_expected1[] = {0xf5d3d585, 0x03b9699d, 0xe785895a, 0x96fdbaaf};
//	unsigned int nist_ecb_128_enc_expected2[] = {0x43b1cd7f, 0x598ece23, 0x881b00e3, 0xed030688};
//	unsigned int nist_ecb_128_enc_expected3[] = {0x7b0c785e, 0x27e8ad3f, 0x82232071, 0x04725dd4};
//
//	unsigned int nist_ecb_256_enc_expected0[] = {0xf3eed1bd, 0xb5d2a03c, 0x064b5a7e, 0x3db181f8};
//	unsigned int nist_ecb_256_enc_expected1[] = {0x591ccb10, 0xd410ed26, 0xdc5ba74a, 0x31362870};
//	unsigned int nist_ecb_256_enc_expected2[] = {0xb6ed21b9, 0x9ca6f4f9, 0xf153e7b1, 0xbeafed1d};
//	unsigned int nist_ecb_256_enc_expected3[] = {0x23304b7a, 0x39f9f3ff, 0x067d8d8f, 0x9e24ecc7};

	unsigned int result [4];

    ecb_mode_single_block_test(AES_ENCIPHER, nist_aes128_key, AES_128_BIT_KEY,
                               nist_plaintext0, &result);

}


volatile unsigned int* SCAN_BASE = (volatile unsigned int*)0x43c00000;

unsigned create_hw_snapshot(unsigned to, unsigned size) {
    SCAN_BASE[0] = (to+size);
    SCAN_BASE[1] = to;
    SCAN_BASE[2] = size;

    SCAN_BASE[3] = 1;
    SCAN_BASE[3] = 0;

    poll_scan_irq();
    ack_irq();
    while(SCAN_BASE[4] == 0) {
    	asm("nop");
    }

    SCAN_BASE[0] = to;
    SCAN_BASE[1] = (to+size);
    SCAN_BASE[2] = size;

    SCAN_BASE[3] = 1;
    SCAN_BASE[3] = 0;

    poll_scan_irq();
    ack_irq();
    while(SCAN_BASE[4] == 0) {
    	asm("nop");
    }

    return to;
}

void load_hw_snapshot(unsigned from, unsigned to, unsigned size) {
    SCAN_BASE[0] = from;
    SCAN_BASE[1] = to;
    SCAN_BASE[2] = size;

    SCAN_BASE[3] = 1;
    SCAN_BASE[3] = 0;

    poll_scan_irq();
    ack_irq();
    while(SCAN_BASE[4] == 0) {
    	asm("nop");
    }
}

volatile unsigned int* IRQ_BASE = (volatile unsigned int*)0x43c20000;

void poll_scan_irq() {
	while( (IRQ_BASE[0] & 0xD) != 8) {
		asm("nop");
	}
}

void ack_irq() {
	IRQ_BASE[0] = IRQ_BASE[0] & (0x4);
	IRQ_BASE[0] = IRQ_BASE[0] & (~0x4);
}

int main()
{
    init_uart();
    Xil_DCacheDisable();
    Xil_ICacheDisable();

    print("Hello World\n\r");

	unsigned int hash[8] = {0};

	unsigned int snp1_hash[8] = {0xba7816bf, 0x8f01cfea, 0x414140de, 0x5dae2223, 0xb00361a3, 0x96177a9c, 0xb410ff61, 0xf20015ad};
	unsigned int snp2_hash[8] = {0xe3b0c442, 0x98fc1c14, 0x9afbf4c8, 0x996fb924, 0x27ae41e4, 0x649b934c, 0xa495991b, 0x7852b855};

    // Compute hash for "abc", expect snp1_hash as result
    test_sha256(0);

    // Save hw snapshot at 0x10000000 with size 1088
    unsigned snp1 = create_hw_snapshot(0x10000000, 1088);

    // Read digest and check that it is as expected : snp1_hash
    read_digest(hash);
	if(memcmp(hash, snp1_hash, 32) != 0) {
		//error case
		while(1);
	}

	// Change hardware state by computing hash for ""
    test_sha256(1);

    // Reload snapshot 1
    unsigned int snp2 = 0x10008000;
    load_hw_snapshot(snp1, snp2, 1088);

    // SNapshot 1 has been reloaded, so expected result is snp1_hash and not snp2_hash
    // Read digest and check that it is as expected : snp1_hash
    read_digest(hash);
	if(memcmp(hash, snp1_hash, 32) != 0) {
		//error case
		while(1);
	}

    cleanup_platform();
    return 0;
}
