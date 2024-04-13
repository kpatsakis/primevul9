soup_cookie_jar_add_cookie (SoupCookieJar *jar, SoupCookie *cookie)
{
	SoupCookieJarPrivate *priv;
	GSList *old_cookies, *oc, *last = NULL;
	SoupCookie *old_cookie;

	g_return_if_fail (SOUP_IS_COOKIE_JAR (jar));
	g_return_if_fail (cookie != NULL);

	/* Never accept cookies for public domains. */
	if (!g_hostname_is_ip_address (cookie->domain) &&
	    soup_tld_domain_is_public_suffix (cookie->domain)) {
		soup_cookie_free (cookie);
		return;
	}

	priv = soup_cookie_jar_get_instance_private (jar);
	old_cookies = g_hash_table_lookup (priv->domains, cookie->domain);
	for (oc = old_cookies; oc; oc = oc->next) {
		old_cookie = oc->data;
		if (!strcmp (cookie->name, old_cookie->name) &&
		    !g_strcmp0 (cookie->path, old_cookie->path)) {
			if (cookie->expires && soup_date_is_past (cookie->expires)) {
				/* The new cookie has an expired date,
				 * this is the way the the server has
				 * of telling us that we have to
				 * remove the cookie.
				 */
				old_cookies = g_slist_delete_link (old_cookies, oc);
				g_hash_table_insert (priv->domains,
						     g_strdup (cookie->domain),
						     old_cookies);
				soup_cookie_jar_changed (jar, old_cookie, NULL);
				soup_cookie_free (old_cookie);
				soup_cookie_free (cookie);
			} else {
				oc->data = cookie;
				soup_cookie_jar_changed (jar, old_cookie, cookie);
				soup_cookie_free (old_cookie);
			}

			return;
		}
		last = oc;
	}

	/* The new cookie is... a new cookie */
	if (cookie->expires && soup_date_is_past (cookie->expires)) {
		soup_cookie_free (cookie);
		return;
	}

	if (last)
		last->next = g_slist_append (NULL, cookie);
	else {
		old_cookies = g_slist_append (NULL, cookie);
		g_hash_table_insert (priv->domains, g_strdup (cookie->domain),
				     old_cookies);
	}

	soup_cookie_jar_changed (jar, NULL, cookie);
}