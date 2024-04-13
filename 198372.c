unsigned gnutls_session_get_flags(gnutls_session_t session)
{
	unsigned flags = 0;

	if (gnutls_safe_renegotiation_status(session))
		flags |= GNUTLS_SFLAGS_SAFE_RENEGOTIATION;
	if (gnutls_session_ext_master_secret_status(session))
		flags |= GNUTLS_SFLAGS_EXT_MASTER_SECRET;
	if (gnutls_session_etm_status(session))
		flags |= GNUTLS_SFLAGS_ETM;
	if (gnutls_heartbeat_allowed(session, GNUTLS_HB_LOCAL_ALLOWED_TO_SEND))
		flags |= GNUTLS_SFLAGS_HB_LOCAL_SEND;
	if (gnutls_heartbeat_allowed(session, GNUTLS_HB_PEER_ALLOWED_TO_SEND))
		flags |= GNUTLS_SFLAGS_HB_PEER_SEND;
	if (session->internals.false_start_used)
		flags |= GNUTLS_SFLAGS_FALSE_START;

	return flags;
}