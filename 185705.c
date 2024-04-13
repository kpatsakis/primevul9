soup_cookie_jar_get_accept_policy (SoupCookieJar *jar)
{
	SoupCookieJarPrivate *priv;

	g_return_val_if_fail (SOUP_IS_COOKIE_JAR (jar), SOUP_COOKIE_JAR_ACCEPT_ALWAYS);

	priv = soup_cookie_jar_get_instance_private (jar);
	return priv->accept_policy;
}