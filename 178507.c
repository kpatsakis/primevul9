static gboolean session_write_msg_cb(SockInfo *source, GIOCondition condition,
				     gpointer data)
{
	Session *session = SESSION(data);
	gint ret;

	cm_return_val_if_fail(condition == G_IO_OUT, FALSE);
	cm_return_val_if_fail(session->write_buf != NULL, FALSE);
	cm_return_val_if_fail(session->write_buf_p != NULL, FALSE);
	cm_return_val_if_fail(session->write_buf_len > 0, FALSE);

	ret = session_write_buf(session);

	if (ret < 0) {
		session->state = SESSION_ERROR;
		return FALSE;
	} else if (ret > 0)
		return TRUE;

	if (session->io_tag > 0) {
		g_source_remove(session->io_tag);
		session->io_tag = 0;
	}

	session_recv_msg(session);

	return FALSE;
}