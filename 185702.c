process_set_cookie_header (SoupMessage *msg, gpointer user_data)
{
	SoupCookieJar *jar = user_data;
	SoupCookieJarPrivate *priv = soup_cookie_jar_get_instance_private (jar);
	GSList *new_cookies, *nc;

	if (priv->accept_policy == SOUP_COOKIE_JAR_ACCEPT_NEVER)
		return;

	new_cookies = soup_cookies_from_response (msg);
	for (nc = new_cookies; nc; nc = nc->next) {
		SoupURI *first_party = soup_message_get_first_party (msg);
		
		if ((priv->accept_policy == SOUP_COOKIE_JAR_ACCEPT_NO_THIRD_PARTY &&
		     !incoming_cookie_is_third_party (nc->data, first_party)) ||
		    priv->accept_policy == SOUP_COOKIE_JAR_ACCEPT_ALWAYS)
			soup_cookie_jar_add_cookie (jar, nc->data);
		else
			soup_cookie_free (nc->data);
	}
	g_slist_free (new_cookies);
}