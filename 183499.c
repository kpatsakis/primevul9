mac_init(gnutls_mac_algorithm_t mac, opaque * secret, int secret_size,
	 int ver)
{
    mac_hd_t td;

    if (mac == GNUTLS_MAC_NULL)
	return GNUTLS_MAC_FAILED;

    if (ver == GNUTLS_SSL3) {	/* SSL 3.0 */
	td = _gnutls_mac_init_ssl3(mac, secret, secret_size);
    } else {			/* TLS 1.x */
	td = _gnutls_hmac_init(mac, secret, secret_size);
    }

    return td;
}