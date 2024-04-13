openssl_dec(const EVP_CIPHER * cipher, const unsigned char *key, const unsigned char *iv,
		const unsigned char *input, size_t length, unsigned char *output)
{
	int r = SC_ERROR_INTERNAL;
	EVP_CIPHER_CTX * ctx = NULL;
	int outl = 0;
	int outl_tmp = 0;
	unsigned char iv_tmp[EVP_MAX_IV_LENGTH] = { 0 };

	memcpy(iv_tmp, iv, EVP_MAX_IV_LENGTH);
	ctx = EVP_CIPHER_CTX_new();
	if (ctx == NULL)
		goto out;
	EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv_tmp);
	EVP_CIPHER_CTX_set_padding(ctx, 0);

	if (!EVP_DecryptUpdate(ctx, output, &outl, input, length))
		goto out;

	if (!EVP_DecryptFinal_ex(ctx, output + outl, &outl_tmp))
		goto out;

	r = SC_SUCCESS;
out:
	if (ctx)
		EVP_CIPHER_CTX_free(ctx);
	return r;
}