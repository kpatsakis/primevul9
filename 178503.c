void session_set_access_time(Session *session)
{
	session->last_access_time = time(NULL);
}