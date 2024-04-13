aes128_encrypt_cbc(const unsigned char *key, int keysize, unsigned char iv[16],
		const unsigned char *input, size_t length, unsigned char *output)
{
	return openssl_enc(EVP_aes_128_cbc(), key, iv, input, length, output);
}