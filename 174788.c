sso_ntlm_close (SoupAuthNTLMPrivate *priv)
{
	if (priv->fd_in != -1) {
		close (priv->fd_in);
		priv->fd_in = -1;
	}

	if (priv->fd_out != -1) {
		close (priv->fd_out);
		priv->fd_out = -1;
	}
}