soup_cookie_jar_get_property (GObject *object, guint prop_id,
			      GValue *value, GParamSpec *pspec)
{
	SoupCookieJarPrivate *priv =
		soup_cookie_jar_get_instance_private (SOUP_COOKIE_JAR (object));

	switch (prop_id) {
	case PROP_READ_ONLY:
		g_value_set_boolean (value, priv->read_only);
		break;
	case PROP_ACCEPT_POLICY:
		g_value_set_enum (value, priv->accept_policy);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}