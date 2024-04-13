static int rds_getname(struct socket *sock, struct sockaddr *uaddr,
		       int *uaddr_len, int peer)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)uaddr;
	struct rds_sock *rs = rds_sk_to_rs(sock->sk);

	memset(sin->sin_zero, 0, sizeof(sin->sin_zero));

	/* racey, don't care */
	if (peer) {
		if (!rs->rs_conn_addr)
			return -ENOTCONN;

		sin->sin_port = rs->rs_conn_port;
		sin->sin_addr.s_addr = rs->rs_conn_addr;
	} else {
		sin->sin_port = rs->rs_bound_port;
		sin->sin_addr.s_addr = rs->rs_bound_addr;
	}

	sin->sin_family = AF_INET;

	*uaddr_len = sizeof(*sin);
	return 0;
}