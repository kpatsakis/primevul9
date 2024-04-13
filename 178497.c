gboolean session_is_running(Session *session)
{
	return (session->state == SESSION_READY ||
		session->state == SESSION_SEND ||
		session->state == SESSION_RECV);
}