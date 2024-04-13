static int check_if_sorted(gnutls_x509_crt_t * crt, int nr)
{
	void *prev_dn = NULL;
	void *dn;
	size_t prev_dn_size = 0, dn_size;
	int i, ret;

	/* check if the X.509 list is ordered */
	if (nr > 1) {
		for (i = 0; i < nr; i++) {
			if (i > 0) {
				dn = crt[i]->raw_dn.data;
				dn_size = crt[i]->raw_dn.size;

				if (dn_size != prev_dn_size
				    || memcmp(dn, prev_dn, dn_size) != 0) {
					ret =
					    gnutls_assert_val
					    (GNUTLS_E_CERTIFICATE_LIST_UNSORTED);
					goto cleanup;
				}
			}

			prev_dn = crt[i]->raw_issuer_dn.data;
			prev_dn_size = crt[i]->raw_issuer_dn.size;
		}
	}
	ret = 0;

cleanup:
	return ret;
}