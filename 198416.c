des3_encrypt_ecb(const unsigned char *key, int keysize,
		const unsigned char *input, int length, unsigned char *output)
{
	unsigned char iv[EVP_MAX_IV_LENGTH] = { 0 };
	unsigned char bKey[24] = { 0 };

	if (keysize == 16) {
		memcpy(&bKey[0], key, 16);
		memcpy(&bKey[16], key, 8);
	}
	else {
		memcpy(&bKey[0], key, 24);
	}

	return openssl_enc(EVP_des_ede3(), bKey, iv, input, length, output);
}