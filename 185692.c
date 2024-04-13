soup_cookie_jar_class_init (SoupCookieJarClass *jar_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (jar_class);

	object_class->constructed = soup_cookie_jar_constructed;
	object_class->finalize = soup_cookie_jar_finalize;
	object_class->set_property = soup_cookie_jar_set_property;
	object_class->get_property = soup_cookie_jar_get_property;

	jar_class->is_persistent = soup_cookie_jar_real_is_persistent;

	/**
	 * SoupCookieJar::changed:
	 * @jar: the #SoupCookieJar
	 * @old_cookie: the old #SoupCookie value
	 * @new_cookie: the new #SoupCookie value
	 *
	 * Emitted when @jar changes. If a cookie has been added,
	 * @new_cookie will contain the newly-added cookie and
	 * @old_cookie will be %NULL. If a cookie has been deleted,
	 * @old_cookie will contain the to-be-deleted cookie and
	 * @new_cookie will be %NULL. If a cookie has been changed,
	 * @old_cookie will contain its old value, and @new_cookie its
	 * new value.
	 **/
	signals[CHANGED] =
		g_signal_new ("changed",
			      G_OBJECT_CLASS_TYPE (object_class),
			      G_SIGNAL_RUN_FIRST,
			      G_STRUCT_OFFSET (SoupCookieJarClass, changed),
			      NULL, NULL,
			      NULL,
			      G_TYPE_NONE, 2, 
			      SOUP_TYPE_COOKIE | G_SIGNAL_TYPE_STATIC_SCOPE,
			      SOUP_TYPE_COOKIE | G_SIGNAL_TYPE_STATIC_SCOPE);

	/**
	 * SOUP_COOKIE_JAR_READ_ONLY:
	 *
	 * Alias for the #SoupCookieJar:read-only property. (Whether
	 * or not the cookie jar is read-only.)
	 **/
	g_object_class_install_property (
		object_class, PROP_READ_ONLY,
		g_param_spec_boolean (SOUP_COOKIE_JAR_READ_ONLY,
				      "Read-only",
				      "Whether or not the cookie jar is read-only",
				      FALSE,
				      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * SOUP_COOKIE_JAR_ACCEPT_POLICY:
	 *
	 * Alias for the #SoupCookieJar:accept-policy property.
	 *
	 * Since: 2.30
	 */
	/**
	 * SoupCookieJar:accept-policy:
	 *
	 * The policy the jar should follow to accept or reject cookies
	 *
	 * Since: 2.30
	 */
	g_object_class_install_property (
		object_class, PROP_ACCEPT_POLICY,
		g_param_spec_enum (SOUP_COOKIE_JAR_ACCEPT_POLICY,
				   "Accept-policy",
				   "The policy the jar should follow to accept or reject cookies",
				   SOUP_TYPE_COOKIE_JAR_ACCEPT_POLICY,
				   SOUP_COOKIE_JAR_ACCEPT_ALWAYS,
				   G_PARAM_READWRITE));
}