static gint session_write_buf(Session *session)
{
	gint write_len;
	gint to_write_len;

	cm_return_val_if_fail(session->write_buf != NULL, -1);
	cm_return_val_if_fail(session->write_buf_p != NULL, -1);
	cm_return_val_if_fail(session->write_buf_len > 0, -1);

	to_write_len = session->write_buf_len -
		(session->write_buf_p - session->write_buf);
	to_write_len = MIN(to_write_len, SESSION_BUFFSIZE);

	write_len = sock_write(session->sock, session->write_buf_p,
			       to_write_len);

	if (write_len < 0) {
		switch (errno) {
		case EAGAIN:
			write_len = 0;
			break;
		default:
			g_warning("sock_write: %s", g_strerror(errno));
			session->state = SESSION_ERROR;
			return -1;
		}
	}

	/* incomplete write */
	if (session->write_buf_p - session->write_buf + write_len <
	    session->write_buf_len) {
		session->write_buf_p += write_len;
		return 1;
	}

	g_free(session->write_buf);
	session->write_buf = NULL;
	session->write_buf_p = NULL;
	session->write_buf_len = 0;

	return 0;
}