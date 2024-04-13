soup_cookie_jar_set_cookie_with_first_party (SoupCookieJar *jar,
					     SoupURI *uri,
					     SoupURI *first_party,
					     const char *cookie)
{
	SoupCookie *soup_cookie;

	g_return_if_fail (SOUP_IS_COOKIE_JAR (jar));
	g_return_if_fail (uri != NULL);
	g_return_if_fail (first_party != NULL);
	g_return_if_fail (cookie != NULL);

	if (!uri->host)
		return;

	soup_cookie = soup_cookie_parse (cookie, uri);
	if (soup_cookie)
		soup_cookie_jar_add_cookie_with_first_party (jar, first_party, soup_cookie);
}