msg_starting_cb (SoupMessage *msg, gpointer feature)
{
	SoupCookieJar *jar = SOUP_COOKIE_JAR (feature);
	char *cookies;

	cookies = soup_cookie_jar_get_cookies (jar, soup_message_get_uri (msg), TRUE);
	if (cookies) {
		soup_message_headers_replace (msg->request_headers,
					      "Cookie", cookies);
		g_free (cookies);
	} else
		soup_message_headers_remove (msg->request_headers, "Cookie");
}