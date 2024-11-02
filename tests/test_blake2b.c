#include "blake2b.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct
{
	char *input;		// Input value
	char *key;			// Input value
	char *expected; // Expected result
} TEST_CASES[] = {
		{"abc", "", "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923"},
		{"", "", "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce"},
		{"The quick brown fox jumps over the lazy dog", "", "a8add4bdddfd93e4877d2746e62817b116364a1fa7bc148d95090bc7333b3673f82401cf7aa2e4cb1ecd90296e3f14cb5413f8ed77be73045b13914cdcd6a918"}};

int test_hex(const char *hexstr, const uint8_t *hash, size_t hashlen)
{
	size_t hexstrlen = strlen(hexstr);
	const uint8_t *ptr = (const uint8_t *)hash;
	char curr_hexstr[2];

	for (size_t i = 0; i < hashlen && i < hexstrlen; i++)
	{
		memcpy(curr_hexstr, (hexstr + i * 2), 2);

		uint8_t hex = (uint8_t)strtol(curr_hexstr, NULL, 16);

		if (*(ptr + i) != hex)
			return 0;
	}

	return 1;
}

// Function to run the tests
void test_blake2b()
{
	int num_tests = sizeof(TEST_CASES) / sizeof(TEST_CASES[0]);
	int passed_tests = 0;

	uint8_t output[BLAKE2B_OUTBYTES];

	for (int i = 0; i < num_tests; i++)
	{
		int inlen = strlen(TEST_CASES[i].input);
		int keylen = strlen(TEST_CASES[i].key);

		blake2b(output, BLAKE2B_OUTBYTES, TEST_CASES[i].input, inlen, TEST_CASES[i].key, keylen);

		if (test_hex(TEST_CASES[i].expected, output, BLAKE2B_OUTBYTES))
		{
			printf("Test %i passed\n", i + 1);
			passed_tests++;
		}
		else
		{
			printf("Test %i failed\n", i + 1);
		}
	}

	printf("\n%d/%d tests passed.\n", passed_tests, num_tests);
}

int main()
{
	// Run all tests in the module
	test_blake2b();
	return 0;
}
