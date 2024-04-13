static BIGNUM * cf2bn(const u8 *buf, size_t bufsize, BIGNUM *num)
{
	u8 tmp[512];

	invert_buf(tmp, buf, bufsize);

	return BN_bin2bn(tmp, bufsize, num);
}