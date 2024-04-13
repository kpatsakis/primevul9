int _gnutls_decrypt(gnutls_session_t session, opaque * ciphertext,
		    size_t ciphertext_size, uint8 * data,
		    size_t max_data_size, content_type_t type)
{
    gnutls_datum_t gtxt;
    gnutls_datum_t gcipher;
    int ret;

    if (ciphertext_size == 0)
	return 0;

    gcipher.size = ciphertext_size;
    gcipher.data = ciphertext;

    ret =
	_gnutls_ciphertext2compressed(session, data, max_data_size,
				      gcipher, type);
    if (ret < 0) {
	return ret;
    }

    if (ret == 0 || is_read_comp_null(session) == 0) {
	/* ret == ret */

    } else {
	gnutls_datum_t gcomp;

	/* compression has this malloc overhead.
	 */

	gcomp.data = data;
	gcomp.size = ret;
	ret = _gnutls_m_compressed2plaintext(session, &gtxt, gcomp);
	if (ret < 0) {
	    return ret;
	}

	if (gtxt.size > max_data_size) {
	    gnutls_assert();
	    _gnutls_free_datum(&gtxt);
	    /* This shouldn't have happen and
	     * is a TLS fatal error.
	     */
	    return GNUTLS_E_INTERNAL_ERROR;
	}

	memcpy(data, gtxt.data, gtxt.size);
	ret = gtxt.size;

	_gnutls_free_datum(&gtxt);
    }

    return ret;
}