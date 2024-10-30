#include "blake2b.h"
#include "utils.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_hexads(const void *hash)
{
	uint8_t *ptr = (uint8_t *)hash;

	while (*ptr)
		printf("%02x", *ptr++);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Input required\n");
		return -1;
	}

	uint8_t *input = argv[1];
	size_t inlen = strlen((char *)input);

	uint8_t *key = argc > 2 ? argv[2] : "";
	size_t keylen = strlen((char *)key);

	uint8_t output[BLAKE2B_OUTBYTES];

	blake2b(output, BLAKE2B_OUTBYTES, input, inlen, key, keylen);

	print_hexads(output);
	printf("\n");

	return 0;
}
