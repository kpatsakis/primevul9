void negotiate(CLIENT *client) {
	char zeros[300];
	u64 size_host;

	memset(zeros, 0, 290);
	if (write(client->net, INIT_PASSWD, 8) < 0)
		err("Negotiation failed: %m");
	cliserv_magic = htonll(cliserv_magic);
	if (write(client->net, &cliserv_magic, sizeof(cliserv_magic)) < 0)
		err("Negotiation failed: %m");
	size_host = htonll((u64)(client->exportsize));
	if (write(client->net, &size_host, 8) < 0)
		err("Negotiation failed: %m");
	if (write(client->net, zeros, 128) < 0)
		err("Negotiation failed: %m");
}