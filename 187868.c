_gnutls_generate_early_secrets_for_psk(gnutls_session_t session)
{
	const uint8_t *psk;
	size_t psk_size;
	const mac_entry_st *prf;
	int ret;

	psk = session->key.binders[0].psk.data;
	psk_size = session->key.binders[0].psk.size;
	prf = session->key.binders[0].prf;

	if (unlikely(psk_size == 0))
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	ret = _tls13_init_secret2(prf, psk, psk_size,
				  session->key.proto.tls13.temp_secret);
	if (ret < 0)
		return gnutls_assert_val(ret);

	session->key.proto.tls13.temp_secret_size = prf->output_size;

	ret = generate_early_secrets(session, session->key.binders[0].prf);
	if (ret < 0)
		return gnutls_assert_val(ret);

	return 0;
}