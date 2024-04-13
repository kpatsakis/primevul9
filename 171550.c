gnutls_x509_crt_list_import(gnutls_x509_crt_t * certs,
			    unsigned int *cert_max,
			    const gnutls_datum_t * data,
			    gnutls_x509_crt_fmt_t format,
			    unsigned int flags)
{
	int size;
	const char *ptr;
	gnutls_datum_t tmp;
	int ret, nocopy = 0;
	unsigned int count = 0, j;

	if (format == GNUTLS_X509_FMT_DER) {
		if (*cert_max < 1) {
			*cert_max = 1;
			return GNUTLS_E_SHORT_MEMORY_BUFFER;
		}

		count = 1;	/* import only the first one */

		ret = gnutls_x509_crt_init(&certs[0]);
		if (ret < 0) {
			gnutls_assert();
			goto error;
		}

		ret = gnutls_x509_crt_import(certs[0], data, format);
		if (ret < 0) {
			gnutls_assert();
			goto error;
		}

		*cert_max = 1;
		return 1;
	}

	/* move to the certificate
	 */
	ptr = memmem(data->data, data->size,
		     PEM_CERT_SEP, sizeof(PEM_CERT_SEP) - 1);
	if (ptr == NULL)
		ptr = memmem(data->data, data->size,
			     PEM_CERT_SEP2, sizeof(PEM_CERT_SEP2) - 1);

	if (ptr == NULL)
		return gnutls_assert_val(GNUTLS_E_NO_CERTIFICATE_FOUND);

	count = 0;

	do {
		if (count >= *cert_max) {
			if (!
			    (flags &
			     GNUTLS_X509_CRT_LIST_IMPORT_FAIL_IF_EXCEED))
				break;
			else
				nocopy = 1;
		}

		if (!nocopy) {
			ret = gnutls_x509_crt_init(&certs[count]);
			if (ret < 0) {
				gnutls_assert();
				goto error;
			}

			tmp.data = (void *) ptr;
			tmp.size =
			    data->size - (ptr - (char *) data->data);

			ret =
			    gnutls_x509_crt_import(certs[count], &tmp,
						   GNUTLS_X509_FMT_PEM);
			if (ret < 0) {
				gnutls_assert();
				goto error;
			}
		}

		/* now we move ptr after the pem header 
		 */
		ptr++;
		/* find the next certificate (if any)
		 */
		size = data->size - (ptr - (char *) data->data);

		if (size > 0) {
			char *ptr2;

			ptr2 =
			    memmem(ptr, size, PEM_CERT_SEP,
				   sizeof(PEM_CERT_SEP) - 1);
			if (ptr2 == NULL)
				ptr2 = memmem(ptr, size, PEM_CERT_SEP2,
					      sizeof(PEM_CERT_SEP2) - 1);

			ptr = ptr2;
		} else
			ptr = NULL;

		count++;
	}
	while (ptr != NULL);

	*cert_max = count;

	if (flags & GNUTLS_X509_CRT_LIST_FAIL_IF_UNSORTED) {
		ret = check_if_sorted(certs, *cert_max);
		if (ret < 0) {
			gnutls_assert();
			goto error;
		}
	}

	if (nocopy == 0)
		return count;
	else
		return GNUTLS_E_SHORT_MEMORY_BUFFER;

      error:
	for (j = 0; j < count; j++)
		gnutls_x509_crt_deinit(certs[j]);
	return ret;
}