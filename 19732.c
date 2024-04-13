_cdf_tole8(uint64_t sv)
{
	uint64_t rv;
	uint8_t *s = RCAST(uint8_t *, RCAST(void *, &sv));
	uint8_t *d = RCAST(uint8_t *, RCAST(void *, &rv));
	d[0] = s[7];
	d[1] = s[6];
	d[2] = s[5];
	d[3] = s[4];
	d[4] = s[3];
	d[5] = s[2];
	d[6] = s[1];
	d[7] = s[0];
	return rv;
}