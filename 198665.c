static int encode_public_key(RSA *rsa, u8 *key, size_t *keysize)
{
	u8 buf[1024], *p = buf;
	u8 bnbuf[256];
	int base = 0;
	int r;
	const BIGNUM *rsa_n, *rsa_e;

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
	*p++ = (5 * base + 7) >> 8;
	*p++ = (5 * base + 7) & 0xFF;
	*p++ = opt_key_num;

	RSA_get0_key(rsa, &rsa_n, &rsa_e, NULL);
	r = bn2cf(rsa_n, bnbuf);
	if (r != 2*base) {
		fprintf(stderr, "Invalid public key.\n");
		return 2;
	}
	memcpy(p, bnbuf, 2*base);
	p += 2*base;

	memset(p, 0, base);
	p += base;

	memset(bnbuf, 0, 2*base);
	memcpy(p, bnbuf, 2*base);
	p += 2*base;
	r = bn2cf(rsa_e, bnbuf);
	memcpy(p, bnbuf, 4);
	p += 4;

	memcpy(key, buf, p - buf);
	*keysize = p - buf;

	return 0;
}