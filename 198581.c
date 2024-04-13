static int encode_private_key(RSA *rsa, u8 *key, size_t *keysize)
{
	u8 buf[1024], *p = buf;
	u8 bnbuf[256];
	int base = 0;
	int r;
	const BIGNUM *rsa_p, *rsa_q, *rsa_dmp1, *rsa_dmq1, *rsa_iqmp;

	switch (RSA_bits(rsa)) {
	case 512:
		base = 32;
		break;
	case 768:
		base = 48;
		break;
	case 1024:
		base = 64;
		break;
	case 2048:
		base = 128;
		break;
	}
	if (base == 0) {
		fprintf(stderr, "Key length invalid.\n");
		return 2;
	}
	*p++ = (5 * base + 3) >> 8;
	*p++ = (5 * base + 3) & 0xFF;
	*p++ = opt_key_num;

	RSA_get0_factors(rsa, &rsa_p, &rsa_q);

	r = bn2cf(rsa_p, bnbuf);
	if (r != base) {
		fprintf(stderr, "Invalid private key.\n");
		return 2;
	}
	memcpy(p, bnbuf, base);
	p += base;

	r = bn2cf(rsa_q, bnbuf);
	if (r != base) {
		fprintf(stderr, "Invalid private key.\n");
		return 2;
	}
	memcpy(p, bnbuf, base);
	p += base;

	RSA_get0_crt_params(rsa, &rsa_dmp1, &rsa_dmq1, &rsa_iqmp);

	r = bn2cf(rsa_iqmp, bnbuf);
	if (r != base) {
		fprintf(stderr, "Invalid private key.\n");
		return 2;
	}
	memcpy(p, bnbuf, base);
	p += base;

	r = bn2cf(rsa_dmp1, bnbuf);
	if (r != base) {
		fprintf(stderr, "Invalid private key.\n");
		return 2;
	}
	memcpy(p, bnbuf, base);
	p += base;

	r = bn2cf(rsa_dmq1, bnbuf);
	if (r != base) {
		fprintf(stderr, "Invalid private key.\n");
		return 2;
	}
	memcpy(p, bnbuf, base);
	p += base;

	memcpy(key, buf, p - buf);
	*keysize = p - buf;

	return 0;
}