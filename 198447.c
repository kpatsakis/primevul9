static int parse_private_key(const u8 *key, size_t keysize, RSA *rsa)
{
	const u8 *p = key;
	BIGNUM *bn_p, *q, *dmp1, *dmq1, *iqmp;
	int base;

	base = (keysize - 3) / 5;
	if (base != 32 && base != 48 && base != 64 && base != 128) {
		fprintf(stderr, "Invalid private key.\n");
		return -1;
	}
	p += 3;
	bn_p = BN_new();
	if (bn_p == NULL)
		return -1;
	cf2bn(p, base, bn_p);
	p += base;

	q = BN_new();
	if (q == NULL)
		return -1;
	cf2bn(p, base, q);
	p += base;

	iqmp = BN_new();
	if (iqmp == NULL)
		return -1;
	cf2bn(p, base, iqmp);
	p += base;

	dmp1 = BN_new();
	if (dmp1 == NULL)
		return -1;
	cf2bn(p, base, dmp1);
	p += base;

	dmq1 = BN_new();
	if (dmq1 == NULL)
		return -1;
	cf2bn(p, base, dmq1);
	p += base;

	
	if (RSA_set0_factors(rsa, bn_p, q) != 1)
		return -1;
	if (RSA_set0_crt_params(rsa, dmp1, dmq1, iqmp) != 1)
		return -1;
	if (gen_d(rsa))
		return -1;

	return 0;
}