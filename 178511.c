gint session_send_msg(Session *session, const gchar *msg)
{
	gboolean ret;

	cm_return_val_if_fail(session->write_buf == NULL, -1);
	cm_return_val_if_fail(msg != NULL, -1);

	session->state = SESSION_SEND;
	session->write_buf = g_strconcat((strlen(msg) > 0 ? msg : ""), "\r\n", NULL);
	session->write_buf_p = session->write_buf;
	session->write_buf_len = strlen(msg) + 2;

	ret = session_write_msg_cb(session->sock, G_IO_OUT, session);

	if (ret == TRUE)
		session->io_tag = sock_add_watch(session->sock, G_IO_OUT,
						 session_write_msg_cb, session);
	else if (session->state == SESSION_ERROR)
		return -1;

	return 0;
}