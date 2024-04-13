gnutls_x509_crt_get_issuer_dn_by_oid(gnutls_x509_crt_t cert,
				     const char *oid, int indx,
				     unsigned int raw_flag, void *buf,
				     size_t * buf_size)
{
	gnutls_datum_t td;
	int ret;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_x509_parse_dn_oid(cert->cert,
					"tbsCertificate.issuer.rdnSequence",
					oid, indx, raw_flag, &td);
	if (ret < 0)
		return gnutls_assert_val(ret);

	return _gnutls_strdatum_to_buf(&td, buf, buf_size);
}