calc_enc_length(gnutls_session_t session, int data_size,
		int hash_size, uint8 * pad, int random_pad,
		cipher_type_t block_algo, uint16 blocksize)
{
    uint8 rnd;
    int length;

    *pad = 0;

    switch (block_algo) {
    case CIPHER_STREAM:
	length = data_size + hash_size;

	break;
    case CIPHER_BLOCK:
	if (_gnutls_get_random(&rnd, 1, GNUTLS_WEAK_RANDOM) < 0) {
	    gnutls_assert();
	    return GNUTLS_E_MEMORY_ERROR;
	}

	/* make rnd a multiple of blocksize */
	if (session->security_parameters.version == GNUTLS_SSL3 ||
	    random_pad == 0) {
	    rnd = 0;
	} else {
	    rnd = (rnd / blocksize) * blocksize;
	    /* added to avoid the case of pad calculated 0
	     * seen below for pad calculation.
	     */
	    if (rnd > blocksize)
		rnd -= blocksize;
	}

	length = data_size + hash_size;

	*pad = (uint8) (blocksize - (length % blocksize)) + rnd;

	length += *pad;
	if (session->security_parameters.version >= GNUTLS_TLS1_1)
	    length += blocksize;	/* for the IV */

	break;
    default:
	gnutls_assert();
	return GNUTLS_E_INTERNAL_ERROR;
    }

    return length;
}