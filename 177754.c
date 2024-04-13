int _gnutls_alt_name_process(gnutls_datum_t *out, unsigned type, const gnutls_datum_t *san, unsigned raw)
{
	int ret;
	if (type == GNUTLS_SAN_DNSNAME && !raw) {
		ret = gnutls_idna_map((char*)san->data, san->size, out, 0);
		if (ret < 0) {
			return gnutls_assert_val(ret);
		}
	} else if (type == GNUTLS_SAN_RFC822NAME && !raw) {
		ret = _gnutls_idna_email_map((char*)san->data, san->size, out);
		if (ret < 0) {
			return gnutls_assert_val(ret);
		}
	} else if (type == GNUTLS_SAN_URI && !raw) {
		if (!_gnutls_str_is_print((char*)san->data, san->size)) {
			_gnutls_debug_log("non-ASCII URIs are not supported\n");
			return gnutls_assert_val(GNUTLS_E_UNIMPLEMENTED_FEATURE);
		} else {
			ret = _gnutls_set_strdatum(out, san->data, san->size);
			if (ret < 0)
				return gnutls_assert_val(ret);
		}
	} else {
		ret = _gnutls_set_strdatum(out, san->data, san->size);
		if (ret < 0)
			return gnutls_assert_val(ret);
	}

	return 0;
}