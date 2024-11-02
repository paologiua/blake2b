#include "blake2b.h"
#include <stdio.h>
#include <string.h>

void print_hexads(const void *hash, size_t hashlen)
{
	const uint8_t *ptr = (const uint8_t *)hash;

	for (size_t i = 0; i < hashlen; i++)
		printf("%02x", *(ptr + i));
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Use %s [input] [key?]\n", argv[0]);
		return -1;
	}

	uint8_t *input = (uint8_t *)argv[1];
	size_t inlen = strlen((char *)input);

	uint8_t *key = (uint8_t *)(argc > 2 ? argv[2] : "");
	size_t keylen = strlen((char *)key);

	uint8_t output[BLAKE2B_OUTBYTES];

	blake2b(output, BLAKE2B_OUTBYTES, input, inlen, key, keylen);

	print_hexads(output, BLAKE2B_OUTBYTES);
	printf("\n");

	return 0;
}
