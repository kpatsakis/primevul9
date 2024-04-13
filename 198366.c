void gnutls_deinit(gnutls_session_t session)
{
	unsigned int i;

	if (session == NULL)
		return;

	/* remove auth info firstly */
	_gnutls_free_auth_info(session);

	_gnutls_handshake_internal_state_clear(session);
	_gnutls_handshake_io_buffer_clear(session);
	_gnutls_ext_free_session_data(session);

	for (i = 0; i < MAX_EPOCH_INDEX; i++)
		if (session->record_parameters[i] != NULL) {
			_gnutls_epoch_free(session,
					   session->record_parameters[i]);
			session->record_parameters[i] = NULL;
		}

	_gnutls_buffer_clear(&session->internals.handshake_hash_buffer);
	_gnutls_buffer_clear(&session->internals.hb_remote_data);
	_gnutls_buffer_clear(&session->internals.hb_local_data);
	_gnutls_buffer_clear(&session->internals.record_presend_buffer);

	_mbuffer_head_clear(&session->internals.record_buffer);
	_mbuffer_head_clear(&session->internals.record_recv_buffer);
	_mbuffer_head_clear(&session->internals.record_send_buffer);

	_gnutls_free_datum(&session->internals.resumption_data);

	gnutls_free(session->internals.rexts);

	gnutls_free(session->internals.rsup);

	gnutls_credentials_clear(session);
	_gnutls_selected_certs_deinit(session);

	gnutls_free(session);
}