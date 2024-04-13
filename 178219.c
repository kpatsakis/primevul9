soup_auth_ntlm_free_connection_state (SoupConnectionAuth *auth,
				      gpointer state)
{
	SoupNTLMConnectionState *conn = state;

	g_free (conn->nonce);
	g_free (conn->response_header);
	g_slice_free (SoupNTLMConnectionState, conn);
}