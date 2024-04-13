ssh_create_socket(int privileged, struct addrinfo *ai)
{
	int sock, r, gaierr;
	struct addrinfo hints, *res = NULL;

	sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sock < 0) {
		error("socket: %s", strerror(errno));
		return -1;
	}
	fcntl(sock, F_SETFD, FD_CLOEXEC);

	/* Bind the socket to an alternative local IP address */
	if (options.bind_address == NULL && !privileged)
		return sock;

	if (options.bind_address) {
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = ai->ai_family;
		hints.ai_socktype = ai->ai_socktype;
		hints.ai_protocol = ai->ai_protocol;
		hints.ai_flags = AI_PASSIVE;
		gaierr = getaddrinfo(options.bind_address, NULL, &hints, &res);
		if (gaierr) {
			error("getaddrinfo: %s: %s", options.bind_address,
			    ssh_gai_strerror(gaierr));
			close(sock);
			return -1;
		}
	}
	/*
	 * If we are running as root and want to connect to a privileged
	 * port, bind our own socket to a privileged port.
	 */
	if (privileged) {
		PRIV_START;
		r = bindresvport_sa(sock, res ? res->ai_addr : NULL);
		PRIV_END;
		if (r < 0) {
			error("bindresvport_sa: af=%d %s", ai->ai_family,
			    strerror(errno));
			goto fail;
		}
	} else {
		if (bind(sock, res->ai_addr, res->ai_addrlen) < 0) {
			error("bind: %s: %s", options.bind_address,
			    strerror(errno));
 fail:
			close(sock);
			freeaddrinfo(res);
			return -1;
		}
	}
	if (res != NULL)
		freeaddrinfo(res);
	return sock;
}