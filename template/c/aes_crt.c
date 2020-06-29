/*
 * aes_crt.c
 *
 *  Created on: Jun 5, 2019
 *      Author: nasm
 */


#include <stdio.h>
#include <string.h>

#include "aes_crt.h"

uint32_t readFromAddress(uint32_t pAddress) {
    return *((volatile uint32_t *)pAddress);
}

void writeToAddress(uint32_t pAddress, uint32_t pData) {
    *((volatile uint32_t *)pAddress) = pData;
}


void readMultiFromAddress(uint32_t pAddress, uint32_t *pData, int block_size) {
    for(unsigned int i = 0; i < block_size; ++i)
        pData[(block_size - 1) - i] = readFromAddress(pAddress + (i * BYTES_PER_WORD));
}

void writeMultiToAddress(uint32_t pAddress, uint32_t *pData, int block_size) {
    for(unsigned int i = 0; i < block_size; ++i)
        writeToAddress(pAddress + (((block_size - 1) - i) * BYTES_PER_WORD), pData[i]);
}

void writeMulticharToAddress(uint32_t pAddress, char *pData, int block_byte_size) {

    uint32_t wdata;
    int block_ptr = 0;
    int block_size = block_byte_size / BYTES_PER_WORD ;

    for(int i = (block_byte_size-1); i >= 0; i--) {
        wdata = ( wdata << 8 ) | ((*pData++) & 0xFF);

        if (i % BYTES_PER_WORD == 0)  {
            writeToAddress(pAddress + (((block_size - 1) - block_ptr) * BYTES_PER_WORD), wdata);
            block_ptr++;
        }
    }
}

void my_delay(int wait_cycles) {
    for (int i=0; i<wait_cycles; i++)
        asm volatile ("nop \n\t") ;
}

int verifyMulti(uint32_t *expectedData, uint32_t *receivedData, int block_size) {
    int match = 1;

    for (int i=0; i<block_size; i++)
    {
        if (expectedData[i] != receivedData[i])
            return 0;
    }

    return match;
}

void aes_encrypt(uint32_t *pt, uint32_t *st, uint32_t *ct, uint32_t key_sel) {

    // Write the inputs
    writeToAddress(AES_KEY_SEL, key_sel);
    writeMultiToAddress(AES_PT_BASE, pt, AES_PT_WORDS);
    writeMultiToAddress(AES_ST_BASE, st, AES_ST_WORDS);

    // Start the AES encryption
    writeToAddress(AES_START, 0x0);
    writeToAddress(AES_START, 0x1);
    // Wait to see that START has been asserted to make sure DONE has a valid value
    while (readFromAddress(AES_START) == 0) {
        my_delay(AES_START_DELAY);
    }
    writeToAddress(AES_START, 0x0);

    // Wait for valid output
    while (readFromAddress(AES_DONE) == 0) {
        my_delay(AES_DONE_DELAY);
    }

    // Read the Encrypted data
    readMultiFromAddress(AES_CT_BASE, ct, AES_CT_WORDS);

}

int check_aes()
{
  //// Give a test input and verify AES enryption
    printf("    Verifying AES crypto engine ...\n") ;

    writeToAddress(AES_KEY0_BASE, 0x28aed9a6);
    writeToAddress(AES_KEY0_BASE+4, 0x207e1516);
    writeToAddress(AES_KEY0_BASE+8, 0x09c94f3c);
    writeToAddress(AES_KEY0_BASE+12, 0xa6f71558);
    writeToAddress(AES_KEY0_BASE+16, 0x28aef2a6);
	writeToAddress(AES_KEY0_BASE+20, 0x2b3e1216);

    // Input for AES encyption
    uint32_t pt[4]  = {0x00001111, 0x22223333, 0x44445555, 0x66667777};
    uint32_t st[4]  = {0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734};
    uint32_t key[6] = {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c, 0x2b7e1516, 0x28aed2a6};
    uint32_t ct[AES_CT_WORDS];
    //uint32_t expectedCt[AES_CT_WORDS] = {0x4fcb8db8, 0x5784a2c1, 0xbb77db7e, 0xde3217ac};
    uint32_t expectedCt[AES_CT_WORDS] = {0x4fcb9ca9, 0x75a691f2, 0xff338e2b, 0xb85460db};

    uint32_t key_sel = 0;

    int aes_working;

    // Write the AES key
    writeMultiToAddress(AES_KEY0_BASE, key, AES_KEY_WORDS);
    writeMultiToAddress(AES_KEY1_BASE, key, AES_KEY_WORDS);
    writeMultiToAddress(AES_KEY2_BASE, key, AES_KEY_WORDS);

    // call the aes encryption function
    aes_encrypt(pt, st, ct, key_sel);

    // Verify the Encrypted data
    aes_working = verifyMulti(expectedCt, ct, AES_CT_WORDS);

    if (aes_working)
        printf("    AES engine encryption successfully verified\n");
    else
        printf("    AES engine failed, disabling the crypto engine !\n");

    return aes_working ;
}
