imapx_server_set_connection_timeout (GIOStream *connection,
				     gint timeout_seconds)
{
	GSocket *socket;
	gint previous_timeout = -1;

	if (G_IS_TLS_CONNECTION (connection)) {
		GIOStream *base_io_stream = NULL;

		g_object_get (G_OBJECT (connection), "base-io-stream", &base_io_stream, NULL);

		connection = base_io_stream;
	} else if (connection) {
		/* Connection can be NULL, when a custom command (GSubProcess) is used instead */
		g_object_ref (connection);
	}

	if (!G_IS_SOCKET_CONNECTION (connection)) {
		g_clear_object (&connection);
		return previous_timeout;
	}

	socket = g_socket_connection_get_socket (G_SOCKET_CONNECTION (connection));
	if (socket) {
		previous_timeout = g_socket_get_timeout (socket);
		g_socket_set_timeout (socket, timeout_seconds);
	}

	g_clear_object (&connection);

	return previous_timeout;
}