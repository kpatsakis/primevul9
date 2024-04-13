gint session_disconnect(Session *session)
{
	session_close(session);
	return 0;
}