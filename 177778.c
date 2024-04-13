int gnutls_x509_aki_set_cert_issuer(gnutls_x509_aki_t aki,
				    unsigned int san_type,
				    const gnutls_datum_t *san,
				    const char *othername_oid,
				    const gnutls_datum_t *serial)
{
	int ret;
	gnutls_datum_t t_san, t_othername_oid = { NULL, 0 };

	ret = _gnutls_set_datum(&aki->serial, serial->data, serial->size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	aki->cert_issuer.names[aki->cert_issuer.size].type = san_type;

	ret = _gnutls_set_strdatum(&t_san, san->data, san->size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (othername_oid) {
		t_othername_oid.data = (uint8_t *) gnutls_strdup(othername_oid);
		if (t_othername_oid.data == NULL) {
			gnutls_free(t_san.data);
			return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
		}
		t_othername_oid.size = strlen(othername_oid);
	}

	ret =
	    subject_alt_names_set(&aki->cert_issuer.names,
				  &aki->cert_issuer.size, san_type, &t_san,
				  (char *)t_othername_oid.data, 0);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return 0;
}