cdf_namecmp(const char *d, const uint16_t *s, size_t l)
{
	for (; l--; d++, s++)
		if (*d != CDF_TOLE2(*s))
			return CAST(unsigned char, *d) - CDF_TOLE2(*s);
	return 0;
}