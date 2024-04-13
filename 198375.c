inline static void deinit_internal_params(gnutls_session_t session)
{
#if defined(ENABLE_DHE) || defined(ENABLE_ANON)
	if (session->internals.params.free_dh_params)
		gnutls_dh_params_deinit(session->internals.params.
					dh_params);
#endif

	_gnutls_handshake_hash_buffers_clear(session);

	memset(&session->internals.params, 0,
	       sizeof(session->internals.params));
}