cdf_copy_info(cdf_property_info_t *inp, const void *p, const void *e,
    size_t len)
{
	if (inp->pi_type & CDF_VECTOR)
		return 0;

	if (CAST(size_t, CAST(const char *, e) - CAST(const char *, p)) < len)
		return 0;

	(void)memcpy(&inp->pi_val, p, len);

	switch (len) {
	case 2:
		inp->pi_u16 = CDF_TOLE2(inp->pi_u16);
		break;
	case 4:
		inp->pi_u32 = CDF_TOLE4(inp->pi_u32);
		break;
	case 8:
		inp->pi_u64 = CDF_TOLE8(inp->pi_u64);
		break;
	default:
		abort();
	}
	return 1;
}