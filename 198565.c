static int bn2cf(const BIGNUM *num, u8 *buf)
{
	u8 tmp[512];
	int r;

	r = BN_bn2bin(num, tmp);
	if (r <= 0)
		return r;
	invert_buf(buf, tmp, r);

	return r;
}