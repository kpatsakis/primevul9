soup_auth_ntlm_is_authenticated (SoupAuth *auth)
{
	SoupAuthNTLM *auth_ntlm = SOUP_AUTH_NTLM (auth);
	SoupAuthNTLMPrivate *priv = soup_auth_ntlm_get_instance_private (auth_ntlm);

	return (priv->password_state != SOUP_NTLM_PASSWORD_NONE &&
		priv->password_state != SOUP_NTLM_PASSWORD_REJECTED);
}