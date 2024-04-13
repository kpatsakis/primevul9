ssh_connect(const char *host, struct addrinfo *addrs,
    struct sockaddr_storage *hostaddr, u_short port, int family,
    int connection_attempts, int *timeout_ms, int want_keepalive, int needpriv)
{
	if (options.proxy_command == NULL) {
		return ssh_connect_direct(host, addrs, hostaddr, port, family,
		    connection_attempts, timeout_ms, want_keepalive, needpriv);
	} else if (strcmp(options.proxy_command, "-") == 0) {
		packet_set_connection(STDIN_FILENO, STDOUT_FILENO);
		return 0; /* Always succeeds */
	} else if (options.proxy_use_fdpass) {
		return ssh_proxy_fdpass_connect(host, port,
		    options.proxy_command);
	}
	return ssh_proxy_connect(host, port, options.proxy_command);
}