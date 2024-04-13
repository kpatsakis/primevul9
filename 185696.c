soup_cookie_jar_delete_cookie (SoupCookieJar *jar,
			       SoupCookie    *cookie)
{
	SoupCookieJarPrivate *priv;
	GSList *cookies, *p;

	g_return_if_fail (SOUP_IS_COOKIE_JAR (jar));
	g_return_if_fail (cookie != NULL);

	priv = soup_cookie_jar_get_instance_private (jar);

	cookies = g_hash_table_lookup (priv->domains, cookie->domain);
	if (cookies == NULL)
		return;

	for (p = cookies; p; p = p->next ) {
		SoupCookie *c = (SoupCookie*)p->data;
		if (soup_cookie_equal (cookie, c)) {
			cookies = g_slist_delete_link (cookies, p);
			g_hash_table_insert (priv->domains,
					     g_strdup (cookie->domain),
					     cookies);
			soup_cookie_jar_changed (jar, c, NULL);
			soup_cookie_free (c);
			return;
		}
	}
}