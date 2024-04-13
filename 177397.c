static int rds_connect(struct socket *sock, struct sockaddr *uaddr,
		       int addr_len, int flags)
{
	struct sock *sk = sock->sk;
	struct sockaddr_in *sin = (struct sockaddr_in *)uaddr;
	struct rds_sock *rs = rds_sk_to_rs(sk);
	int ret = 0;

	lock_sock(sk);

	if (addr_len != sizeof(struct sockaddr_in)) {
		ret = -EINVAL;
		goto out;
	}

	if (sin->sin_family != AF_INET) {
		ret = -EAFNOSUPPORT;
		goto out;
	}

	if (sin->sin_addr.s_addr == htonl(INADDR_ANY)) {
		ret = -EDESTADDRREQ;
		goto out;
	}

	rs->rs_conn_addr = sin->sin_addr.s_addr;
	rs->rs_conn_port = sin->sin_port;

out:
	release_sock(sk);
	return ret;
}