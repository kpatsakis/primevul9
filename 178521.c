static gboolean session_read_data_cb(SockInfo *source, GIOCondition condition,
				     gpointer data)
{
	Session *session = SESSION(data);
	GByteArray *data_buf;
	gint terminator_len;
	gboolean complete = FALSE;
	guint data_len;
	gint ret;

	cm_return_val_if_fail(condition == G_IO_IN, FALSE);

	session_set_timeout(session, session->timeout_interval);

	if (session->read_buf_len == 0) {
		gint read_len;

		read_len = sock_read(session->sock, session->read_buf,
				     SESSION_BUFFSIZE);

		if (read_len == 0) {
			g_warning("sock_read: received EOF");
			session->state = SESSION_EOF;
			return FALSE;
		}

		if (read_len < 0) {
			switch (errno) {
			case EAGAIN:
				return TRUE;
			default:
				g_warning("sock_read: %s", g_strerror(errno));
				session->state = SESSION_ERROR;
				return FALSE;
			}
		}

		session->read_buf_len = read_len;
	}

	data_buf = session->read_data_buf;
	terminator_len = strlen(session->read_data_terminator);

	if (session->read_buf_len == 0)
		return TRUE;

	g_byte_array_append(data_buf, session->read_buf_p,
			    session->read_buf_len);

	session->read_buf_len = 0;
	session->read_buf_p = session->read_buf;

	/* check if data is terminated */
	if (data_buf->len >= terminator_len) {
		if (memcmp(data_buf->data, session->read_data_terminator,
			   terminator_len) == 0)
			complete = TRUE;
		else if (data_buf->len >= terminator_len + 2 &&
			 memcmp(data_buf->data + data_buf->len -
				(terminator_len + 2), "\r\n", 2) == 0 &&
			 memcmp(data_buf->data + data_buf->len -
				terminator_len, session->read_data_terminator,
				terminator_len) == 0)
			complete = TRUE;
	}

	/* incomplete read */
	if (!complete) {
		GDateTime *tv_cur = g_date_time_new_now_local();

		GTimeSpan ts = g_date_time_difference(tv_cur, session->tv_prev);
                if (1000 - ts < 0 || ts > UI_REFRESH_INTERVAL) {
                        session->recv_data_progressive_notify
                                (session, data_buf->len, 0,
                                 session->recv_data_progressive_notify_data);
			g_date_time_unref(session->tv_prev);
                        session->tv_prev = g_date_time_new_now_local();
                }
		g_date_time_unref(tv_cur);
		return TRUE;
	}

	/* complete */
	if (session->io_tag > 0) {
		g_source_remove(session->io_tag);
		session->io_tag = 0;
	}

	data_len = data_buf->len - terminator_len;

	/* callback */
	ret = session->recv_data_finished(session, (gchar *)data_buf->data,
					  data_len);

	g_byte_array_set_size(data_buf, 0);

	session->recv_data_notify(session, data_len,
				  session->recv_data_notify_data);

	if (ret < 0)
		session->state = SESSION_ERROR;

	return FALSE;
}