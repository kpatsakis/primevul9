static int _gnutls_psk_recv_params(gnutls_session_t session,
				   const unsigned char *data, size_t len)
{
	unsigned i;
	gnutls_psk_server_credentials_t pskcred;
	const version_entry_st *vers = get_version(session);
	int ret;

	if (!vers || !vers->tls13_sem)
		return 0;

	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		if (session->internals.hsk_flags & HSK_PSK_KE_MODES_SENT) {
			uint16_t selected_identity = _gnutls_read_uint16(data);

			for (i=0;i<sizeof(session->key.binders)/sizeof(session->key.binders[0]);i++) {
				if (session->key.binders[i].prf != NULL && session->key.binders[i].idx == selected_identity) {
					if (session->key.binders[i].resumption) {
						session->internals.resumed = true;
						_gnutls_handshake_log("EXT[%p]: selected PSK-resumption mode\n", session);
					} else {
						_gnutls_handshake_log("EXT[%p]: selected PSK mode\n", session);
					}

					/* different PSK is selected, than the one we calculated early secrets */
					if (i != 0) {
						/* ensure that selected binder is set on (our) index zero */
						swap_binders(session);

						ret = _gnutls_generate_early_secrets_for_psk(session);
						if (ret < 0)
							return gnutls_assert_val(ret);
					}
					session->internals.hsk_flags |= HSK_PSK_SELECTED;
				}
			}

			return 0;
		} else {
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_EXTENSION);
		}
	} else {
		if (session->internals.hsk_flags & HSK_PSK_KE_MODES_RECEIVED) {
			if (session->internals.hsk_flags & HSK_PSK_KE_MODE_INVALID) {
				/* We received a "psk_ke_modes" extension, but with a value we don't support */
				return 0;
			}

			pskcred = (gnutls_psk_server_credentials_t)
					_gnutls_get_cred(session, GNUTLS_CRD_PSK);

			/* If there are no PSK credentials, this extension is not applicable,
			 * so we return zero. */
			if (pskcred == NULL && (session->internals.flags & GNUTLS_NO_TICKETS))
				return 0;

			return server_recv_params(session, data, len, pskcred);
		} else {
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_EXTENSION);
		}
	}
}