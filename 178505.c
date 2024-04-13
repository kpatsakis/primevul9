void session_set_timeout(Session *session, guint interval)
{
	if (session->timeout_tag > 0)
		g_source_remove(session->timeout_tag);

	session->timeout_interval = interval;
	if (interval > 0) {
		if (interval % 1000 == 0)
			session->timeout_tag =
				g_timeout_add_seconds(interval/1000, session_timeout_cb, session);
		else
			session->timeout_tag =
				g_timeout_add(interval, session_timeout_cb, session);
	} else
		session->timeout_tag = 0;
}