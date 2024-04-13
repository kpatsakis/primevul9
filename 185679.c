soup_cookie_jar_changed (SoupCookieJar *jar,
			 SoupCookie *old, SoupCookie *new)
{
	SoupCookieJarPrivate *priv = soup_cookie_jar_get_instance_private (jar);

	if (old && old != new)
		g_hash_table_remove (priv->serials, old);
	if (new) {
		priv->serial++;
		g_hash_table_insert (priv->serials, new, GUINT_TO_POINTER (priv->serial));
	}

	if (priv->read_only || !priv->constructed)
		return;

	g_signal_emit (jar, signals[CHANGED], 0, old, new);
}