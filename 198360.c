gnutls_handshake_set_private_extensions(gnutls_session_t session,
					int allow)
{
	session->internals.enable_private = allow;
}