static gboolean client_timeout(gpointer user_data)
{
	struct tcp_partial_client_data *client = user_data;
	int sock;

	sock = g_io_channel_unix_get_fd(client->channel);

	debug("client %d timeout pending %d bytes", sock, client->buf_end);

	g_hash_table_remove(partial_tcp_req_table, GINT_TO_POINTER(sock));

	return FALSE;
}