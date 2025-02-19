static char *is_string_at(RCore *core, ut64 addr, int *olen) {
	ut8 rstr[128] = {0};
	int ret = 0, len = 0;
	ut8 *str = calloc (256, 1);
	if (!str) {
		if (olen) {
			*olen = 0;
		}
		return NULL;
	}
	r_io_read_at (core->io, addr, str, 255);

	str[255] = 0;
	if (is_string (str, 256, &len)) {
		if (olen) {
			*olen = len;
		}
		return (char*) str;
	}

	ut64 *cstr = (ut64*)str;
	ut64 lowptr = cstr[0];
	if (lowptr >> 32) { // must be pa mode only
		lowptr &= UT32_MAX;
	}
	// cstring
	if (cstr[0] == 0 && cstr[1] < 0x1000) {
		ut64 ptr = cstr[2];
		if (ptr >> 32) { // must be pa mode only
			ptr &= UT32_MAX;
		}
		if (ptr) {
			r_io_read_at (core->io, ptr, rstr, sizeof (rstr));
			rstr[127] = 0;
			ret = is_string (rstr, 128, &len);
			if (ret) {
				strcpy ((char*) str, (char*) rstr);
				if (olen) {
					*olen = len;
				}
				return (char*) str;
			}
		}
	} else {
		// pstring
		r_io_read_at (core->io, lowptr, rstr, sizeof (rstr));
		rstr[127] = 0;
		ret = is_string (rstr, sizeof (rstr), &len);
		if (ret) {
			strcpy ((char*) str, (char*) rstr);
			if (olen) {
				*olen = len;
			}
			return (char*) str;
		}
	}
	// check if current section have no exec bit
	if (len < 1) {
		ret = 0;
		free (str);
		len = -1;
	} else if (olen) {
		*olen = len;
	}
	// NOTE: coverity says that ret is always 0 here, so str is dead code
	return ret? (char *)str: NULL;
}