static gboolean tcp_idle_timeout(gpointer user_data)
{
	struct server_data *server = user_data;

	debug("");

	if (!server)
		return FALSE;

	destroy_server(server);

	return FALSE;
}