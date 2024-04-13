sha256_digest(const unsigned char *input, size_t length, unsigned char *output)
{
	return openssl_dig(EVP_sha256(), input, length, output);
}