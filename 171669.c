int sock_common_setsockopt(struct socket *sock, int level, int optname,
			   char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;

	return sk->sk_prot->setsockopt(sk, level, optname, optval, optlen);
}