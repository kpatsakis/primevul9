soup_cookie_jar_get_cookies (SoupCookieJar *jar, SoupURI *uri,
			     gboolean for_http)
{
	GSList *cookies;

	g_return_val_if_fail (SOUP_IS_COOKIE_JAR (jar), NULL);
	g_return_val_if_fail (uri != NULL, NULL);

	cookies = get_cookies (jar, uri, for_http, FALSE);

	if (cookies) {
		char *result = soup_cookies_to_cookie_header (cookies);
		g_slist_free (cookies);

		if (!*result) {
			g_free (result);
			result = NULL;
		}
		return result;
	} else
		return NULL;
}