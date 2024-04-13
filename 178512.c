static gint session_connect_cb(SockInfo *sock, gpointer data)
{
	Session *session = SESSION(data);

	session->conn_id = 0;

	if (!sock) {
		g_warning("can't connect to server.");
		session->state = SESSION_ERROR;
		if (session->connect_finished)
			session->connect_finished(session, FALSE);
		return -1;
	}

	session->sock = sock;
	sock->account = session->account;
	sock->is_smtp = session->is_smtp;
	sock->ssl_cert_auto_accept = session->ssl_cert_auto_accept;

	if (session->proxy_info) {
		debug_print("connecting through socks\n");
		sock_set_nonblocking_mode(sock, FALSE);
		if (proxy_connect(sock, session->server, session->port,
					session->proxy_info) < 0) {
			g_warning("can't establish SOCKS connection.");
			session->state = SESSION_ERROR;
			return -1;
		}
	}


#ifdef USE_GNUTLS
	sock->gnutls_priority = session->gnutls_priority;
	sock->use_tls_sni = session->use_tls_sni;

	if (session->ssl_type == SSL_TUNNEL) {
		sock_set_nonblocking_mode(sock, FALSE);
		if (!ssl_init_socket(sock)) {
			g_warning("can't initialize SSL/TLS.");
			log_error(LOG_PROTOCOL, _("SSL/TLS handshake failed\n"));
			session->state = SESSION_ERROR;
			if (session->connect_finished)
				session->connect_finished(session, FALSE);
			return -1;
		}
	}
#endif

	/* we could have gotten a timeout while waiting for user input in 
	 * an SSL certificate dialog */
	if (session->state == SESSION_TIMEOUT) {
		if (session->connect_finished)
			session->connect_finished(session, FALSE);
		return -1;
	}

	sock_set_nonblocking_mode(sock, session->nonblocking);

	debug_print("session (%p): connected\n", session);

	session->state = SESSION_RECV;
	session->io_tag = sock_add_watch(session->sock, G_IO_IN,
					 session_read_msg_cb,
					 session);

	if (session->connect_finished)
		session->connect_finished(session, TRUE);
	return 0;
}