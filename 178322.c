static void free_partial_reqs(gpointer value)
{
	struct tcp_partial_client_data *data = value;

	client_reset(data);
	g_free(data);
}