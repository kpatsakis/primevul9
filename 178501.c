void session_register_ping(Session *session, gboolean (*ping_cb)(gpointer data))
{
	if (!session)
		return;
	if (session->ping_tag > -1)
		g_source_remove(session->ping_tag);

	session->ping_tag = -1;

	if (ping_cb != NULL)
		session->ping_tag = g_timeout_add_seconds(60, ping_cb, session);
}