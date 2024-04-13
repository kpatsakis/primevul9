generate_early_secrets(gnutls_session_t session,
		       const mac_entry_st *prf)
{
	int ret;

	ret = _tls13_derive_secret2(prf, EARLY_TRAFFIC_LABEL, sizeof(EARLY_TRAFFIC_LABEL)-1,
				    session->internals.handshake_hash_buffer.data,
				    session->internals.handshake_hash_buffer_client_hello_len,
				    session->key.proto.tls13.temp_secret,
				    session->key.proto.tls13.e_ckey);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _gnutls_call_keylog_func(session, "CLIENT_EARLY_TRAFFIC_SECRET",
				       session->key.proto.tls13.e_ckey,
				       prf->output_size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _tls13_derive_secret2(prf, EARLY_EXPORTER_MASTER_LABEL, sizeof(EARLY_EXPORTER_MASTER_LABEL)-1,
				    session->internals.handshake_hash_buffer.data,
				    session->internals.handshake_hash_buffer_client_hello_len,
				    session->key.proto.tls13.temp_secret,
				    session->key.proto.tls13.ap_expkey);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _gnutls_call_keylog_func(session, "EARLY_EXPORTER_SECRET",
				       session->key.proto.tls13.ap_expkey,
				       prf->output_size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	return 0;
}