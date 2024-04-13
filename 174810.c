soup_auth_ntlm_create_connection_state (SoupConnectionAuth *auth)
{
	SoupNTLMConnectionState *conn;

	conn = g_slice_new0 (SoupNTLMConnectionState);
	conn->state = SOUP_NTLM_NEW;

	return conn;
}