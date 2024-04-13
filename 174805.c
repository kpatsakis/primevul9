soup_auth_ntlm_is_connection_ready (SoupConnectionAuth *auth,
				    SoupMessage        *msg,
				    gpointer            state)
{
	SoupAuthNTLM *auth_ntlm = SOUP_AUTH_NTLM (auth);
	SoupAuthNTLMPrivate *priv = soup_auth_ntlm_get_instance_private (auth_ntlm);
	SoupNTLMConnectionState *conn = state;

	if (priv->password_state == SOUP_NTLM_PASSWORD_REJECTED)
		return FALSE;

	if (priv->password_state == SOUP_NTLM_PASSWORD_PROVIDED)
		return TRUE;

	return conn->state != SOUP_NTLM_FAILED;
}