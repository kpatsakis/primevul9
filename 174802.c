sso_ntlm_initiate (SoupAuthNTLMPrivate *priv)
{
	char *argv[9];
	gboolean ret;

	if (!priv->sso_available)
		return FALSE;

	if (!ntlm_auth_available && !ntlm_auth_debug) {
		priv->sso_available = FALSE;
		return FALSE;
	}

	/* Return if ntlm_auth execution process exist already */
	if (priv->fd_in != -1 && priv->fd_out != -1)
		return TRUE;
	else {
		/* Clean all sso data before re-initiate */
		sso_ntlm_close (priv);
	}

	if (ntlm_auth_debug) {
		argv[0] = (char *) g_getenv ("SOUP_NTLM_AUTH_DEBUG");
		if (!*argv[0]) {
			priv->sso_available = FALSE;
			return FALSE;
		}
	} else
		argv[0] = NTLM_AUTH;
	argv[1] = "--helper-protocol";
	argv[2] = "ntlmssp-client-1";
	argv[3] = "--use-cached-creds";
	argv[4] = "--username";
	argv[5] = priv->username;
	argv[6] = priv->domain ? "--domain" : NULL;
	argv[7] = priv->domain;
	argv[8] = NULL;

	ret = g_spawn_async_with_pipes (NULL, argv, NULL,
					G_SPAWN_STDERR_TO_DEV_NULL,
					NULL, NULL,
					NULL, &priv->fd_in, &priv->fd_out,
					NULL, NULL);
	if (!ret)
		priv->sso_available = FALSE;
	return ret;
}