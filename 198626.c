des_encrypt_cbc(const unsigned char *key, int keysize, unsigned char iv[EVP_MAX_IV_LENGTH],
		const unsigned char *input, size_t length, unsigned char *output)
{
	return openssl_enc(EVP_des_cbc(), key, iv, input, length, output);
}