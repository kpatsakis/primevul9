aes128_encrypt_ecb(const unsigned char *key, int keysize,
		const unsigned char *input, size_t length, unsigned char *output)
{
	unsigned char iv[EVP_MAX_IV_LENGTH] = { 0 };
	return openssl_enc(EVP_aes_128_ecb(), key, iv, input, length, output);
}