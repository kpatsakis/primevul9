gint session_connect(Session *session, const gchar *server, gushort port)
{
	session->server = g_strdup(server);
	session->port = port;

	if (session->proxy_info) {
		server = session->proxy_info->proxy_host;
		port = session->proxy_info->proxy_port;
	}

#ifdef G_OS_UNIX
	session->conn_id = sock_connect_async(server, port, session_connect_cb,
					      session);
	if (session->conn_id < 0) {
		g_warning("can't connect to server.");
		session_close(session);
		if (session->connect_finished)
			session->connect_finished(session, FALSE);
		return -1;
	}

	return 0;
#else
	SockInfo *sock;

	sock = sock_connect(server, port);
	if (sock == NULL) {
		g_warning("can't connect to server.");
		session_close(session);
		if (session->connect_finished)
			session->connect_finished(session, FALSE);
		return -1;
	}
	sock->is_smtp = session->is_smtp;

	return session_connect_cb(sock, session);
#endif
}