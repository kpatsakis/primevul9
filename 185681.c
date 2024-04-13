soup_cookie_jar_is_persistent (SoupCookieJar *jar)
{
	g_return_val_if_fail (SOUP_IS_COOKIE_JAR (jar), FALSE);

	return SOUP_COOKIE_JAR_GET_CLASS (jar)->is_persistent (jar);
}