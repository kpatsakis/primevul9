openssl_dig(const EVP_MD * digest, const unsigned char *input, size_t length,
		unsigned char *output)
{
	int r = 0;
	EVP_MD_CTX *ctx = NULL;
	unsigned outl = 0;

	ctx = EVP_MD_CTX_create();
	if (ctx == NULL) {
	    r = SC_ERROR_OUT_OF_MEMORY;
	    goto err;
	}
	    
	EVP_MD_CTX_init(ctx);
	EVP_DigestInit_ex(ctx, digest, NULL);
	if (!EVP_DigestUpdate(ctx, input, length)) {
		r = SC_ERROR_INTERNAL;
		goto err;
	}

	if (!EVP_DigestFinal_ex(ctx, output, &outl)) {
		r = SC_ERROR_INTERNAL;
		goto err;
	}
	r = SC_SUCCESS;
err:
	if (ctx)
		EVP_MD_CTX_destroy(ctx);

	return r;
}