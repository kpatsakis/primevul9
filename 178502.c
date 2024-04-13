static gboolean session_write_data_cb(SockInfo *source,
				      GIOCondition condition, gpointer data)
{
	Session *session = SESSION(data);
	guint write_data_len;
	gint ret;

	cm_return_val_if_fail(condition == G_IO_OUT, FALSE);
	cm_return_val_if_fail(session->write_data != NULL, FALSE);
	cm_return_val_if_fail(session->write_data_p != NULL, FALSE);
	cm_return_val_if_fail(session->write_data_len > 0, FALSE);

	write_data_len = session->write_data_len;

	ret = session_write_data(session);

	if (ret < 0) {
		session->state = SESSION_ERROR;
		return FALSE;
	} else if (ret > 0) {
                GDateTime *tv_cur = g_date_time_new_now_local();

                GTimeSpan ts = g_date_time_difference(tv_cur, session->tv_prev);
                if (1000 - ts < 0 || ts > UI_REFRESH_INTERVAL) {
                        session_set_timeout(session, session->timeout_interval);
                        session->send_data_progressive_notify
                                (session,
                                 session->write_data_p - session->write_data,
                                 write_data_len,
                                 session->send_data_progressive_notify_data);
			g_date_time_unref(session->tv_prev);
                        session->tv_prev = g_date_time_new_now_local();
                }
                g_date_time_unref(tv_cur);
		return TRUE;
	}

	if (session->io_tag > 0) {
		g_source_remove(session->io_tag);
		session->io_tag = 0;
	}

	/* callback */
	ret = session->send_data_finished(session, write_data_len);
	session->send_data_notify(session, write_data_len,
				  session->send_data_notify_data);

	return FALSE;
}