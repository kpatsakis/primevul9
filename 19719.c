_cdf_tole2(uint16_t sv)
{
	uint16_t rv;
	uint8_t *s = RCAST(uint8_t *, RCAST(void *, &sv));
	uint8_t *d = RCAST(uint8_t *, RCAST(void *, &rv));
	d[0] = s[1];
	d[1] = s[0];
	return rv;
}