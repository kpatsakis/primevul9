soup_cookie_jar_finalize (GObject *object)
{
	SoupCookieJarPrivate *priv =
		soup_cookie_jar_get_instance_private (SOUP_COOKIE_JAR (object));
	GHashTableIter iter;
	gpointer key, value;

	g_hash_table_iter_init (&iter, priv->domains);
	while (g_hash_table_iter_next (&iter, &key, &value))
		soup_cookies_free (value);
	g_hash_table_destroy (priv->domains);
	g_hash_table_destroy (priv->serials);

	G_OBJECT_CLASS (soup_cookie_jar_parent_class)->finalize (object);
}