soup_cookie_jar_constructed (GObject *object)
{
	SoupCookieJarPrivate *priv =
		soup_cookie_jar_get_instance_private (SOUP_COOKIE_JAR (object));

	priv->constructed = TRUE;
}