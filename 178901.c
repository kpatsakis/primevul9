static int pptp_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct pppox_sock *po;
	struct pptp_opt *opt;
	int error = 0;

	if (!sk)
		return 0;

	lock_sock(sk);

	if (sock_flag(sk, SOCK_DEAD)) {
		release_sock(sk);
		return -EBADF;
	}

	po = pppox_sk(sk);
	opt = &po->proto.pptp;
	del_chan(po);

	pppox_unbind_sock(sk);
	sk->sk_state = PPPOX_DEAD;

	sock_orphan(sk);
	sock->sk = NULL;

	release_sock(sk);
	sock_put(sk);

	return error;
}