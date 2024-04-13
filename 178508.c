void session_set_recv_message_notify(Session *session,
				     RecvMsgNotify notify_func, gpointer data)
{
	session->recv_msg_notify = notify_func;
	session->recv_msg_notify_data = data;
}