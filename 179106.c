void set_peername(int net, CLIENT *client) {
	struct sockaddr_in addrin;
	int addrinlen = sizeof( addrin );
	char *peername ;

	if (getpeername(net, (struct sockaddr *) &addrin, (socklen_t *)&addrinlen) < 0)
		err("getsockname failed: %m");
	peername = inet_ntoa(addrin.sin_addr);
	client->exportname=g_strdup_printf(client->server->exportname, peername);

	msg4(LOG_INFO, "connect from %s, assigned file is %s", 
	     peername, client->exportname);
	client->clientname=g_strdup(peername);
}