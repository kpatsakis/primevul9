soup_auth_ntlm_init (SoupAuthNTLM *ntlm)
{
#ifdef USE_NTLM_AUTH
	SoupAuthNTLMPrivate *priv = soup_auth_ntlm_get_instance_private (ntlm);
	const char *username = NULL, *slash;

	priv->sso_available = TRUE;
	priv->fd_in = -1;
	priv->fd_out = -1;

	username = getenv ("NTLMUSER");
	if (!username)
		username = g_get_user_name ();

	slash = strpbrk (username, "\\/");
	if (slash) {
		priv->username = g_strdup (slash + 1);
		priv->domain = g_strndup (username, slash - username);
	} else {
		priv->username = g_strdup (username);
		priv->domain = NULL;
	}
#endif
}