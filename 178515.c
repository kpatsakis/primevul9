void session_set_send_data_progressive_notify
					(Session *session,
					 SendDataProgressiveNotify notify_func,
					 gpointer data)
{
	session->send_data_progressive_notify = notify_func;
	session->send_data_progressive_notify_data = data;
}