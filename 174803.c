soup_auth_ntlm_finalize (GObject *object)
{
	SoupAuthNTLMPrivate *priv = soup_auth_ntlm_get_instance_private (SOUP_AUTH_NTLM (object));

	g_free (priv->username);
	g_free (priv->domain);

	memset (priv->nt_hash, 0, sizeof (priv->nt_hash));
	memset (priv->lm_hash, 0, sizeof (priv->lm_hash));

#ifdef USE_NTLM_AUTH
	sso_ntlm_close (priv);
#endif

	G_OBJECT_CLASS (soup_auth_ntlm_parent_class)->finalize (object);
}