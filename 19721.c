cdf_get_property_info_pos(const cdf_stream_t *sst, const cdf_header_t *h,
    const uint8_t *p, const uint8_t *e, size_t i)
{
	size_t tail = (i << 1) + 1;
	size_t ofs;
	const uint8_t *q;

	if (p >= e) {
		DPRINTF(("Past end %p < %p\n", e, p));
		return NULL;
	}
	if (cdf_check_stream_offset(sst, h, p, (tail + 1) * sizeof(uint32_t),
	    __LINE__) == -1)
		return NULL;
	ofs = CDF_GETUINT32(p, tail);
	q = CAST(const uint8_t *, cdf_offset(CAST(const void *, p),
	    ofs - 2 * sizeof(uint32_t)));

	if (q < p) {
		DPRINTF(("Wrapped around %p < %p\n", q, p));
		return NULL;
	}

	if (q >= e) {
		DPRINTF(("Ran off the end %p >= %p\n", q, e));
		return NULL;
	}
	return q;
}