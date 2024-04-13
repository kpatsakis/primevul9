gint session_send_data(Session *session, const guchar *data, guint size)
{
	gboolean ret;

	cm_return_val_if_fail(session->write_data == NULL, -1);
	cm_return_val_if_fail(data != NULL, -1);
	cm_return_val_if_fail(size != 0, -1);

	session->state = SESSION_SEND;

	session->write_data = data;
	session->write_data_p = session->write_data;
	session->write_data_len = size;
	g_date_time_unref(session->tv_prev);
        session->tv_prev = g_date_time_new_now_local();

	ret = session_write_data_cb(session->sock, G_IO_OUT, session);

	if (ret == TRUE)
		session->io_tag = sock_add_watch(session->sock, G_IO_OUT,
						 session_write_data_cb,
						 session);
	else if (session->state == SESSION_ERROR)
		return -1;

	return 0;
}