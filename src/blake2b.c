#include "blake2b.h"
#include "utils.h"
#include <string.h>

/**
 * @brief      The mixing function is called by the compress function, and mixes two
 *             8-byte words from the message into the hash state
 *
 * @param[in]  v            the work vector V
 * @param      a, b, c, d  	indices to 8-byte word entries from the work vector V
 * @param  	   x, y        	two 8-byte word entries from padded message v
 */
static void G(uint64_t v[16], int a, int b, int c, int d, int64_t x, int64_t y)
{
	v[a] = v[a] + v[b] + x;
	v[d] = rotr64(v[d] ^ v[a], 32);

	v[c] = v[c] + v[d];
	v[b] = rotr64(v[b] ^ v[c], 24);

	v[a] = v[a] + v[b] + y;
	v[d] = rotr64(v[d] ^ v[a], 16);

	v[c] = v[c] + v[d];
	v[b] = rotr64(v[b] ^ v[c], 63);
}

/**
 * @brief      The blake2b compress function which takes a full 128-byte chunk
 *             of the input message and mixes it into the ongoing state array
 *
 * @param      ctx    blake2b_ctx instance
 * @param      last   Indicates if this is the last call to the function
 */
static void blake2b_compress(blake2b_ctx *ctx, int last)
{
	uint64_t v[16], m[16];

	for (size_t i = 0; i < 8; i++)
	{ // init work variables
		v[i] = ctx->h[i];
		v[i + 8] = blake2b_iv[i];
	}

	v[12] ^= ctx->t[0]; // low 64 bits of offset
	v[13] ^= ctx->t[1]; // high 64 bits
	if (last)						// last block flag set ?
		v[14] = ~v[14];

	for (size_t i = 0; i < 16; i++) // get little-endian words
		m[i] = load64(&ctx->b[8 * i]);

	for (size_t i = 0; i < 12; i++)
	{ // twelve rounds
		G(v, 0, 4, 8, 12, m[blake2b_sigma[i][0]], m[blake2b_sigma[i][1]]);
		G(v, 1, 5, 9, 13, m[blake2b_sigma[i][2]], m[blake2b_sigma[i][3]]);
		G(v, 2, 6, 10, 14, m[blake2b_sigma[i][4]], m[blake2b_sigma[i][5]]);
		G(v, 3, 7, 11, 15, m[blake2b_sigma[i][6]], m[blake2b_sigma[i][7]]);
		G(v, 0, 5, 10, 15, m[blake2b_sigma[i][8]], m[blake2b_sigma[i][9]]);
		G(v, 1, 6, 11, 12, m[blake2b_sigma[i][10]], m[blake2b_sigma[i][11]]);
		G(v, 2, 7, 8, 13, m[blake2b_sigma[i][12]], m[blake2b_sigma[i][13]]);
		G(v, 3, 4, 9, 14, m[blake2b_sigma[i][14]], m[blake2b_sigma[i][15]]);
	}

	for (size_t i = 0; i < 8; ++i)
		ctx->h[i] ^= v[i] ^ v[i + 8];
}

/**
 * @brief      Initializes blake2b context
 *
 * @param      ctx     blake2b_ctx instance passed by reference
 * @param[in]  outlen  The hash output length
 * @param[in]  key     The key
 * @param[in]  keylen  The key length
 *
 * @return     status code
 */
int blake2b_init(blake2b_ctx *ctx, size_t outlen, const void *key, size_t keylen)
{
	if (outlen == 0 || outlen > BLAKE2B_OUTBYTES)
		return -1; // illegal parameters

	if (keylen > BLAKE2B_KEYBYTES)
		return -1; // illegal parameters

	memset(ctx->b, 0, sizeof(ctx->b));
	ctx->t[0] = 0; // input count low word
	ctx->t[1] = 0; // input count high word
	ctx->c = 0;		 // pointer within buffer
	ctx->outlen = outlen;

	blake2b_param P;
	P.digest_length = (uint8_t)outlen;
	P.key_length = (uint8_t)keylen;
	P.fanout = 1;
	P.depth = 1;
	store32(&P.leaf_length, 0);
	store64(&P.node_offset, 0);
	P.node_depth = 0;
	P.inner_length = 0;
	memset(P.reserved, 0, sizeof(P.reserved));
	memset(P.salt, 0, sizeof(P.salt));
	memset(P.personal, 0, sizeof(P.personal));

	for (size_t i = 0; i < 8; i++)
		ctx->h[i] = blake2b_iv[i];
	for (size_t i = 0; i < 8; i++)
		ctx->h[i] ^= load64((const uint8_t *)(&P) + sizeof(ctx->h[i]) * i);

	if (keylen > 0)
	{
		blake2b_update(ctx, key, keylen);
		ctx->c = BLAKE2B_BLOCKBYTES; // at the end
	}

	return 0;
}

/**
 * @brief      Updates blake2b context
 *
 * @param      ctx    blake2b context instance
 * @param[in]  input  The input buffer
 * @param[in]  inlen  The input lenth
 *
 * @return     status code
 */
int blake2b_update(blake2b_ctx *ctx, const void *input, size_t inlen)
{
	for (size_t i = 0; i < inlen; i++)
	{
		if (ctx->c == BLAKE2B_BLOCKBYTES)
		{														// buffer full ?
			ctx->t[0] += ctx->c;			// add counters
			if (ctx->t[0] < ctx->c)		// carry overflow ?
				ctx->t[1]++;						// high word
			blake2b_compress(ctx, 0); // compress (not last)
			ctx->c = 0;								// counter to zero
		}
		ctx->b[ctx->c++] = ((const uint8_t *)input)[i];
	}

	return 0;
}

/**
 * @brief       Generate the message digest (size given in init).
 *						  Result placed in "output".
 *
 * @param       ctx     blake2b context instance
 * @param[out]  output  The hash output
 *
 * @return      status code
 */
int blake2b_final(blake2b_ctx *ctx, void *output)
{
	ctx->t[0] += ctx->c;		// mark last block offset
	if (ctx->t[0] < ctx->c) // carry overflow
		ctx->t[1]++;					// high word

	while (ctx->c < BLAKE2B_BLOCKBYTES) // fill up with zeros
		ctx->b[ctx->c++] = 0;
	blake2b_compress(ctx, 1); // final block flag = 1

	// little endian convert and store
	for (size_t i = 0; i < ctx->outlen; i++)
	{
		((uint8_t *)output)[i] = (ctx->h[i >> 3] >> (8 * (i & 7))) & 0xFF;
	}
	return 0;
}

/**
 * @brief       The main blake2b function
 *
 * @param[out]  output  The hash output
 * @param[in]   outlen  The hash length
 * @param[in]   input   The message input
 * @param[in]   inlen   The message length
 * @param[in]   key     The key
 * @param[in]   keylen  The key length
 *
 * @return      status code
 */
int blake2b(void *output, size_t outlen,
						const void *input, size_t inlen,
						const void *key, size_t keylen)
{
	blake2b_ctx ctx;

	if (blake2b_init(&ctx, outlen, key, keylen) < 0)
		return -1;
	if (blake2b_update(&ctx, input, inlen) < 0)
		return -1;
	if (blake2b_final(&ctx, output) < 0)
		return -1;

	return 0;
}
