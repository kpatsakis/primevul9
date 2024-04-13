soup_cookie_jar_set_property (GObject *object, guint prop_id,
			      const GValue *value, GParamSpec *pspec)
{
	SoupCookieJarPrivate *priv =
		soup_cookie_jar_get_instance_private (SOUP_COOKIE_JAR (object));

	switch (prop_id) {
	case PROP_READ_ONLY:
		priv->read_only = g_value_get_boolean (value);
		break;
	case PROP_ACCEPT_POLICY:
		priv->accept_policy = g_value_get_enum (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}