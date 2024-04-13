gboolean session_is_connected(Session *session)
{
	return (session->state == SESSION_SEND ||
		session->state == SESSION_RECV);
}