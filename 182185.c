sc_pkcs15_convert_bignum(sc_pkcs15_bignum_t *dst, const void *src)
{
#ifdef ENABLE_OPENSSL
	const BIGNUM *bn = (const BIGNUM *)src;

	if (bn == 0)
		return 0;
	dst->len = BN_num_bytes(bn);
	dst->data = malloc(dst->len);
	if (!dst->data)
		return 0;
	BN_bn2bin(bn, dst->data);
	return 1;
#else
	return 0;
#endif
}