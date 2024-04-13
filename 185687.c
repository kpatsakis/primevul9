soup_cookie_jar_set_cookie (SoupCookieJar *jar, SoupURI *uri,
			    const char *cookie)
{
	SoupCookie *soup_cookie;
	SoupCookieJarPrivate *priv;

	g_return_if_fail (SOUP_IS_COOKIE_JAR (jar));
	g_return_if_fail (uri != NULL);
	g_return_if_fail (cookie != NULL);

	if (!uri->host)
		return;

	priv = soup_cookie_jar_get_instance_private (jar);
	if (priv->accept_policy == SOUP_COOKIE_JAR_ACCEPT_NEVER)
		return;

	g_return_if_fail (priv->accept_policy != SOUP_COOKIE_JAR_ACCEPT_NO_THIRD_PARTY);

	soup_cookie = soup_cookie_parse (cookie, uri);
	if (soup_cookie) {
		/* will steal or free soup_cookie */
		soup_cookie_jar_add_cookie (jar, soup_cookie);
	}
}