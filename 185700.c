soup_cookie_jar_set_accept_policy (SoupCookieJar *jar,
				   SoupCookieJarAcceptPolicy policy)
{
	SoupCookieJarPrivate *priv;

	g_return_if_fail (SOUP_IS_COOKIE_JAR (jar));

	priv = soup_cookie_jar_get_instance_private (jar);

	if (priv->accept_policy != policy) {
		priv->accept_policy = policy;
		g_object_notify (G_OBJECT (jar), SOUP_COOKIE_JAR_ACCEPT_POLICY);
	}
}