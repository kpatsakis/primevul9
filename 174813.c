soup_auth_ntlm_authenticate (SoupAuth *auth, const char *username,
			     const char *password)
{
	SoupAuthNTLM *auth_ntlm = SOUP_AUTH_NTLM (auth);
	SoupAuthNTLMPrivate *priv = soup_auth_ntlm_get_instance_private (auth_ntlm);
	const char *slash;

	g_return_if_fail (username != NULL);
	g_return_if_fail (password != NULL);

	if (priv->username)
		g_free (priv->username);
	if (priv->domain)
		g_free (priv->domain);

	slash = strpbrk (username, "\\/");
	if (slash) {
		priv->domain = g_strndup (username, slash - username);
		priv->username = g_strdup (slash + 1);
	} else {
		priv->domain = g_strdup ("");
		priv->username = g_strdup (username);
	}

	soup_ntlm_nt_hash (password, priv->nt_hash);
	soup_ntlm_lanmanager_hash (password, priv->lm_hash);

	priv->password_state = SOUP_NTLM_PASSWORD_PROVIDED;
}