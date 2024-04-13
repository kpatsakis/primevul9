static gboolean session_timeout_cb(gpointer data)
{
	Session *session = SESSION(data);

	g_warning("session timeout.");

	if (session->io_tag > 0) {
		g_source_remove(session->io_tag);
		session->io_tag = 0;
	}

	session->timeout_tag = 0;
	session->state = SESSION_TIMEOUT;

	return FALSE;
}