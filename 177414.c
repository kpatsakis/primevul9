static int rds_create(struct net *net, struct socket *sock, int protocol,
		      int kern)
{
	struct sock *sk;

	if (sock->type != SOCK_SEQPACKET || protocol)
		return -ESOCKTNOSUPPORT;

	sk = sk_alloc(net, AF_RDS, GFP_ATOMIC, &rds_proto, kern);
	if (!sk)
		return -ENOMEM;

	return __rds_create(sock, sk, protocol);
}