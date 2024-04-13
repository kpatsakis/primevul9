static gboolean session_read_msg_cb(SockInfo *source, GIOCondition condition,
				    gpointer data)
{
	Session *session = SESSION(data);
	gchar buf[SESSION_BUFFSIZE];
	gint line_len;
	gchar *newline;
	gchar *msg;
	gint ret;

	cm_return_val_if_fail(condition == G_IO_IN, FALSE);

	session_set_timeout(session, session->timeout_interval);

	if (session->read_buf_len == 0) {
		gint read_len = -1;

		if (session->sock)
			read_len = sock_read(session->sock, session->read_buf,
				     SESSION_BUFFSIZE - 1);

		if (read_len == -1 && session->state == SESSION_DISCONNECTED) {
			g_warning ("sock_read: session disconnected");
			if (session->io_tag > 0) {
				g_source_remove(session->io_tag);
				session->io_tag = 0;
			}
			return FALSE;
		}
		
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

	if ((newline = memchr(session->read_buf_p, '\n', session->read_buf_len))
		!= NULL)
		line_len = newline - session->read_buf_p + 1;
	else
		line_len = session->read_buf_len;

	if (line_len == 0)
		return TRUE;

	memcpy(buf, session->read_buf_p, line_len);
	buf[line_len] = '\0';

	g_string_append(session->read_msg_buf, buf);

	session->read_buf_len -= line_len;
	if (session->read_buf_len == 0)
		session->read_buf_p = session->read_buf;
	else
		session->read_buf_p += line_len;

	/* incomplete read */
	if (buf[line_len - 1] != '\n')
		return TRUE;

	/* complete */
	if (session->io_tag > 0) {
		g_source_remove(session->io_tag);
		session->io_tag = 0;
	}

	/* callback */
	msg = g_strdup(session->read_msg_buf->str);
	strretchomp(msg);
	g_string_truncate(session->read_msg_buf, 0);

	ret = session->recv_msg(session, msg);
	session->recv_msg_notify(session, msg, session->recv_msg_notify_data);

	g_free(msg);

	if (ret < 0)
		session->state = SESSION_ERROR;

	return FALSE;
}