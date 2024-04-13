static int _gnutls_psk_send_params(gnutls_session_t session,
				   gnutls_buffer_t extdata)
{
	gnutls_psk_client_credentials_t cred = NULL;
	const version_entry_st *vers;

	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		vers = _gnutls_version_max(session);

		if (!vers || !vers->tls13_sem)
			return 0;

		if (session->internals.hsk_flags & HSK_PSK_KE_MODES_SENT) {
			cred = (gnutls_psk_client_credentials_t)
					_gnutls_get_cred(session, GNUTLS_CRD_PSK);
		}

		if ((session->internals.flags & GNUTLS_NO_TICKETS) && !session->internals.priorities->have_psk)
			return 0;

		return client_send_params(session, extdata, cred);
	} else {
		vers = get_version(session);

		if (!vers || !vers->tls13_sem)
			return 0;

		if ((session->internals.flags & GNUTLS_NO_TICKETS) && !session->internals.priorities->have_psk)
			return 0;

		if (session->internals.hsk_flags & HSK_PSK_KE_MODES_RECEIVED)
			return server_send_params(session, extdata);
		else
			return 0;
	}
}