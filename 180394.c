int inet_dccp_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	unsigned char old_state;
	int err;

	lock_sock(sk);

	err = -EINVAL;
	if (sock->state != SS_UNCONNECTED || sock->type != SOCK_DCCP)
		goto out;

	old_state = sk->sk_state;
	if (!((1 << old_state) & (DCCPF_CLOSED | DCCPF_LISTEN)))
		goto out;

	/* Really, if the socket is already in listen state
	 * we can only allow the backlog to be adjusted.
	 */
	if (old_state != DCCP_LISTEN) {
		/*
		 * FIXME: here it probably should be sk->sk_prot->listen_start
		 * see tcp_listen_start
		 */
		err = dccp_listen_start(sk, backlog);
		if (err)
			goto out;
	}
	sk->sk_max_ack_backlog = backlog;
	err = 0;

out:
	release_sock(sk);
	return err;
}