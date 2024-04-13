static int parse_public_key(const u8 *key, size_t keysize, RSA *rsa)
{
	const u8 *p = key;
	BIGNUM *n, *e;
	int base;

	base = (keysize - 7) / 5;
	if (base != 32 && base != 48 && base != 64 && base != 128) {
		fprintf(stderr, "Invalid public key.\n");
		return -1;
	}
	p += 3;
	n = BN_new();
	if (n == NULL)
		return -1;
	cf2bn(p, 2 * base, n);
	p += 2 * base;
	p += base;
	p += 2 * base;
	e = BN_new();
	if (e == NULL)
		return -1;
	cf2bn(p, 4, e);
	if (RSA_set0_key(rsa, n, e, NULL) != 1)
	    return -1;
	return 0;
}