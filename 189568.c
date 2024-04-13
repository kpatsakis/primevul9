ns_client_shuttingdown(ns_client_t *client) {
	return (client->newstate == NS_CLIENTSTATE_FREED);
}