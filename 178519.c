void session_set_send_data_notify(Session *session, SendDataNotify notify_func,
				  gpointer data)
{
	session->send_data_notify = notify_func;
	session->send_data_notify_data = data;
}