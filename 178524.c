static gint session_write_data(Session *session)
{
	gint write_len;
	gint to_write_len;

	cm_return_val_if_fail(session->write_data != NULL, -1);
	cm_return_val_if_fail(session->write_data_p != NULL, -1);
	cm_return_val_if_fail(session->write_data_len > 0, -1);

	to_write_len = session->write_data_len -
		(session->write_data_p - session->write_data);
	to_write_len = MIN(to_write_len, SESSION_BUFFSIZE);

	write_len = sock_write(session->sock, session->write_data_p,
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
	if (session->write_data_p - session->write_data + write_len <
	    session->write_data_len) {
		session->write_data_p += write_len;
		return 1;
	}

	session->write_data = NULL;
	session->write_data_p = NULL;
	session->write_data_len = 0;

	return 0;
}