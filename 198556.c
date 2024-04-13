int sc_pkcs15emu_sc_hsm_get_curve(struct ec_curve **curve, u8 *oid, size_t oidlen)
{
	int i;

	for (i = 0; curves[i].oid.value; i++) {
		if ((curves[i].oid.len == oidlen) && !memcmp(curves[i].oid.value, oid, oidlen)) {
			*curve = &curves[i];
			return SC_SUCCESS;
		}
	}
	return SC_ERROR_INVALID_DATA;
}