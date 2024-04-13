soup_cookie_jar_init (SoupCookieJar *jar)
{
	SoupCookieJarPrivate *priv = soup_cookie_jar_get_instance_private (jar);

	priv->domains = g_hash_table_new_full (soup_str_case_hash,
					       soup_str_case_equal,
					       g_free, NULL);
	priv->serials = g_hash_table_new (NULL, NULL);
	priv->accept_policy = SOUP_COOKIE_JAR_ACCEPT_ALWAYS;
}