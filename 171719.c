static int alg_create(struct net *net, struct socket *sock, int protocol,
		      int kern)
{
	struct sock *sk;
	int err;

	if (sock->type != SOCK_SEQPACKET)
		return -ESOCKTNOSUPPORT;
	if (protocol != 0)
		return -EPROTONOSUPPORT;

	err = -ENOMEM;
	sk = sk_alloc(net, PF_ALG, GFP_KERNEL, &alg_proto, kern);
	if (!sk)
		goto out;

	sock->ops = &alg_proto_ops;
	sock_init_data(sock, sk);

	sk->sk_family = PF_ALG;
	sk->sk_destruct = alg_sock_destruct;

	return 0;
out:
	return err;
}