void session_set_recv_data_notify(Session *session, RecvDataNotify notify_func,
				  gpointer data)
{
	session->recv_data_notify = notify_func;
	session->recv_data_notify_data = data;
}