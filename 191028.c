static int client_get_tcp_info(struct sockaddr_storage *server,
			       struct sockaddr_storage *client)
{
	socklen_t length;
	if (server_fd == -1) {
		return -1;
	}
	length = sizeof(*server);
	if (getsockname(server_fd, (struct sockaddr *)server, &length) != 0) {
		return -1;
	}
	length = sizeof(*client);
	if (getpeername(server_fd, (struct sockaddr *)client, &length) != 0) {
		return -1;
	}
	return 0;
}