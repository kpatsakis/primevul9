static gboolean session_recv_data_idle_cb(gpointer data)
{
	Session *session = SESSION(data);
	gboolean ret;

	ret = session_read_data_cb(session->sock, G_IO_IN, session);

	if (ret == TRUE)
		session->io_tag = sock_add_watch(session->sock, G_IO_IN,
						 session_read_data_cb, session);

	return FALSE;
}