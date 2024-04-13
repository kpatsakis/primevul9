des3_encrypt_cbc(const unsigned char *key, int keysize, unsigned char iv[EVP_MAX_IV_LENGTH],
		const unsigned char *input, size_t length, unsigned char *output)
{
	unsigned char bKey[24] = { 0 };

	if (keysize == 16) {
		memcpy(&bKey[0], key, 16);
		memcpy(&bKey[16], key, 8);
	}
	else {
		memcpy(&bKey[0], key, 24);
	}

	return openssl_enc(EVP_des_ede3_cbc(), bKey, iv, input, length, output);
}