data_sock_create(struct net *net, struct socket *sock, int protocol, int kern)
{
	struct sock *sk;

	if (sock->type != SOCK_DGRAM)
		return -ESOCKTNOSUPPORT;

	sk = sk_alloc(net, PF_ISDN, GFP_KERNEL, &mISDN_proto, kern);
	if (!sk)
		return -ENOMEM;

	sock_init_data(sock, sk);

	sock->ops = &data_sock_ops;
	sock->state = SS_UNCONNECTED;
	sock_reset_flag(sk, SOCK_ZAPPED);

	sk->sk_protocol = protocol;
	sk->sk_state    = MISDN_OPEN;
	mISDN_sock_link(&data_sockets, sk);

	return 0;
}