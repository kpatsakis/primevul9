static int pptp_bind(struct socket *sock, struct sockaddr *uservaddr,
	int sockaddr_len)
{
	struct sock *sk = sock->sk;
	struct sockaddr_pppox *sp = (struct sockaddr_pppox *) uservaddr;
	struct pppox_sock *po = pppox_sk(sk);
	struct pptp_opt *opt = &po->proto.pptp;
	int error = 0;

	if (sockaddr_len < sizeof(struct sockaddr_pppox))
		return -EINVAL;

	lock_sock(sk);

	opt->src_addr = sp->sa_addr.pptp;
	if (add_chan(po))
		error = -EBUSY;

	release_sock(sk);
	return error;
}