soup_cookie_jar_all_cookies (SoupCookieJar *jar)
{
	SoupCookieJarPrivate *priv;
	GHashTableIter iter;
	GSList *l = NULL;
	gpointer key, value;

	g_return_val_if_fail (SOUP_IS_COOKIE_JAR (jar), NULL);

	priv = soup_cookie_jar_get_instance_private (jar);

	g_hash_table_iter_init (&iter, priv->domains);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		GSList *p, *cookies = value;
		for (p = cookies; p; p = p->next)
			l = g_slist_prepend (l, soup_cookie_copy (p->data));
	}

	return l;
}