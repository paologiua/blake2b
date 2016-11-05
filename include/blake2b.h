#ifndef BLAKE_H
#define BLAKE_H

#include <stddef.h>
#include <stdint.h>

/**
 * The BLAKE2b initialization vectors
 */
static const uint64_t blake2b_iv[8] = {
		0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
		0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
		0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
		0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179};

/**
 * Table of permutations
 */
static const uint8_t blake2b_sigma[12][16] = {
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
		{14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
		{11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
		{7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
		{9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
		{2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
		{12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
		{13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
		{6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
		{10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
		{14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};

enum blake2b_constant
{
	BLAKE2B_BLOCKBYTES = 128,
	BLAKE2B_OUTBYTES = 64,
	BLAKE2B_KEYBYTES = 64,
	BLAKE2B_SALTBYTES = 16,
	BLAKE2B_PERSONALBYTES = 16,
};

typedef struct blake2b_param
{
	uint8_t digest_length;									 /* 1 */
	uint8_t key_length;											 /* 2 */
	uint8_t fanout;													 /* 3 */
	uint8_t depth;													 /* 4 */
	uint32_t leaf_length;										 /* 8 */
	uint64_t node_offset;										 /* 16 */
	uint8_t node_depth;											 /* 17 */
	uint8_t inner_length;										 /* 18 */
	uint8_t reserved[14];										 /* 32 */
	uint8_t salt[BLAKE2B_SALTBYTES];				 /* 48 */
	uint8_t personal[BLAKE2B_PERSONALBYTES]; /* 64 */
} blake2b_param;

typedef struct blake2b_ctx
{
	uint8_t b[BLAKE2B_BLOCKBYTES]; 					 // input buffer
	uint64_t h[8];	 												 // chained state
	uint64_t t[2];													 // total number of bytes
	size_t c;																 // pointer for b[]
	size_t outlen;													 // digest size
} blake2b_ctx;

int blake2b_init(blake2b_ctx *ctx, size_t outlen, const void *key,
								 size_t keylen);
int blake2b_update(blake2b_ctx *ctx, const void *in, size_t inlen);
int blake2b_final(blake2b_ctx *ctx, void *out, size_t outlen);
int blake2b(void *out, size_t outlen, const void *in, size_t inlen,
						const void *key, size_t keylen);
#endif
