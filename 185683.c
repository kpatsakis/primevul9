soup_cookie_jar_add_cookie_with_first_party (SoupCookieJar *jar, SoupURI *first_party, SoupCookie *cookie)
{
	SoupCookieJarPrivate *priv;

	g_return_if_fail (SOUP_IS_COOKIE_JAR (jar));
	g_return_if_fail (first_party != NULL);
	g_return_if_fail (cookie != NULL);

	priv = soup_cookie_jar_get_instance_private (jar);
	if (priv->accept_policy == SOUP_COOKIE_JAR_ACCEPT_NEVER) {
		soup_cookie_free (cookie);
		return;
	}

	if (priv->accept_policy == SOUP_COOKIE_JAR_ACCEPT_ALWAYS ||
	    !incoming_cookie_is_third_party (cookie, first_party)) {
		/* will steal or free soup_cookie */
		soup_cookie_jar_add_cookie (jar, cookie);
	} else {
		soup_cookie_free (cookie);
	}
}