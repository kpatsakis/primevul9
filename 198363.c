void _gnutls_handshake_internal_state_clear(gnutls_session_t session)
{
	_gnutls_handshake_internal_state_init(session);

	deinit_internal_params(session);
	deinit_keys(session);

	_gnutls_epoch_gc(session);

	session->internals.handshake_endtime = 0;
	session->internals.handshake_in_progress = 0;

	session->internals.tfo.connect_addrlen = 0;
	session->internals.tfo.connect_only = 0;
}