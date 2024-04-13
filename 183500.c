inline static void mac_deinit(mac_hd_t td, opaque * res, int ver)
{
    if (ver == GNUTLS_SSL3) {	/* SSL 3.0 */
	_gnutls_mac_deinit_ssl3(td, res);
    } else {
	_gnutls_hmac_deinit(td, res);
    }
}