_cdf_tole4(uint32_t sv)
{
	uint32_t rv;
	uint8_t *s = RCAST(uint8_t *, RCAST(void *, &sv));
	uint8_t *d = RCAST(uint8_t *, RCAST(void *, &rv));
	d[0] = s[3];
	d[1] = s[2];
	d[2] = s[1];
	d[3] = s[0];
	return rv;
}