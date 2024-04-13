RZ_API RZ_OWN char *MACH0_(get_name)(struct MACH0_(obj_t) * mo, ut32 stridx, bool filter) {
	size_t i = 0;
	if (!mo->symstr || stridx >= mo->symstrlen) {
		return NULL;
	}
	int len = mo->symstrlen - stridx;
	const char *symstr = (const char *)mo->symstr + stridx;
	for (i = 0; i < len; i++) {
		if ((ut8)(symstr[i] & 0xff) == 0xff || !symstr[i]) {
			len = i;
			break;
		}
	}
	if (len > 0) {
		char *res = rz_str_ndup(symstr, len);
		if (filter) {
			rz_str_filter(res);
		}
		return res;
	}
	return NULL;
}