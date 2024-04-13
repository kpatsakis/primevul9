gint session_recv_data(Session *session, guint size, const gchar *terminator)
{
	cm_return_val_if_fail(session->read_data_buf->len == 0, -1);

	session->state = SESSION_RECV;

	g_free(session->read_data_terminator);
	session->read_data_terminator = g_strdup(terminator);
	g_date_time_unref(session->tv_prev);
        session->tv_prev = g_date_time_new_now_local();

	if (session->read_buf_len > 0)
		g_idle_add(session_recv_data_idle_cb, session);
	else
		session->io_tag = sock_add_watch(session->sock, G_IO_IN,
						 session_read_data_cb, session);

	return 0;
}