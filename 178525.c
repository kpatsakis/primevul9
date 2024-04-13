static gint session_close(Session *session)
{
	cm_return_val_if_fail(session != NULL, -1);

#ifdef G_OS_UNIX
	if (session->conn_id > 0) {
		sock_connect_async_cancel(session->conn_id);
		session->conn_id = 0;
		debug_print("session (%p): connection cancelled\n", session);
	}
#endif

	session_set_timeout(session, 0);

	if (session->io_tag > 0) {
		g_source_remove(session->io_tag);
		session->io_tag = 0;
	}

	if (session->sock) {
		sock_close(session->sock, TRUE);
		session->sock = NULL;
		session->state = SESSION_DISCONNECTED;
		debug_print("session (%p): closed\n", session);
	}

	return 0;
}