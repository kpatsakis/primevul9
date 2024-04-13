int sc_pkcs15emu_sc_hsm_get_curve_oid(sc_cvc_t *cvc, const struct sc_lv_data **oid)
{
	int i;

	for (i = 0; curves[i].oid.value; i++) {
		if ((curves[i].prime.len == cvc->primeOrModuluslen) && !memcmp(curves[i].prime.value, cvc->primeOrModulus, cvc->primeOrModuluslen)) {
			*oid = &curves[i].oid;
			return SC_SUCCESS;
		}
	}
	return SC_ERROR_INVALID_DATA;
}