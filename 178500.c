void session_set_recv_data_progressive_notify
					(Session *session,
					 RecvDataProgressiveNotify notify_func,
					 gpointer data)
{
	session->recv_data_progressive_notify = notify_func,
	session->recv_data_progressive_notify_data = data;
}