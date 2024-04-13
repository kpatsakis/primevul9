gint session_recv_msg(Session *session)
{
	cm_return_val_if_fail(session->read_msg_buf->len == 0, -1);

	session->state = SESSION_RECV;

	if (session->read_buf_len > 0)
		g_idle_add(session_recv_msg_idle_cb, session);
	else
		session->io_tag = sock_add_watch(session->sock, G_IO_IN,
						 session_read_msg_cb, session);

	return 0;
}