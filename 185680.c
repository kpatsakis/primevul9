soup_cookie_jar_get_cookie_list (SoupCookieJar *jar, SoupURI *uri, gboolean for_http)
{
	g_return_val_if_fail (SOUP_IS_COOKIE_JAR (jar), NULL);
	g_return_val_if_fail (uri != NULL, NULL);

	return get_cookies (jar, uri, for_http, TRUE);
}