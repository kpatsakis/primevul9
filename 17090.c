static ut64 read_uleb128(ut8 **p, ut8 *end) {
	const char *error = NULL;
	ut64 v;
	*p = (ut8 *)r_uleb128 (*p, end - *p, &v, &error);
	if (error) {
		eprintf ("%s", error);
		R_FREE (error);
		return UT64_MAX;
	}
	return v;
}