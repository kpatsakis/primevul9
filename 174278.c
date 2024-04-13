static int selinux_sctp_bind_connect(struct sock *sk, int optname,
				     struct sockaddr *address,
				     int addrlen)
{
	int len, err = 0, walk_size = 0;
	void *addr_buf;
	struct sockaddr *addr;
	struct socket *sock;

	if (!selinux_policycap_extsockclass())
		return 0;

	/* Process one or more addresses that may be IPv4 or IPv6 */
	sock = sk->sk_socket;
	addr_buf = address;

	while (walk_size < addrlen) {
		if (walk_size + sizeof(sa_family_t) > addrlen)
			return -EINVAL;

		addr = addr_buf;
		switch (addr->sa_family) {
		case AF_UNSPEC:
		case AF_INET:
			len = sizeof(struct sockaddr_in);
			break;
		case AF_INET6:
			len = sizeof(struct sockaddr_in6);
			break;
		default:
			return -EINVAL;
		}

		if (walk_size + len > addrlen)
			return -EINVAL;

		err = -EINVAL;
		switch (optname) {
		/* Bind checks */
		case SCTP_PRIMARY_ADDR:
		case SCTP_SET_PEER_PRIMARY_ADDR:
		case SCTP_SOCKOPT_BINDX_ADD:
			err = selinux_socket_bind(sock, addr, len);
			break;
		/* Connect checks */
		case SCTP_SOCKOPT_CONNECTX:
		case SCTP_PARAM_SET_PRIMARY:
		case SCTP_PARAM_ADD_IP:
		case SCTP_SENDMSG_CONNECT:
			err = selinux_socket_connect_helper(sock, addr, len);
			if (err)
				return err;

			/* As selinux_sctp_bind_connect() is called by the
			 * SCTP protocol layer, the socket is already locked,
			 * therefore selinux_netlbl_socket_connect_locked() is
			 * is called here. The situations handled are:
			 * sctp_connectx(3), sctp_sendmsg(3), sendmsg(2),
			 * whenever a new IP address is added or when a new
			 * primary address is selected.
			 * Note that an SCTP connect(2) call happens before
			 * the SCTP protocol layer and is handled via
			 * selinux_socket_connect().
			 */
			err = selinux_netlbl_socket_connect_locked(sk, addr);
			break;
		}

		if (err)
			return err;

		addr_buf += len;
		walk_size += len;
	}

	return 0;
}