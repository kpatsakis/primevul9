int compat_sock_common_setsockopt(struct socket *sock, int level, int optname,
				  char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;

	if (sk->sk_prot->compat_setsockopt != NULL)
		return sk->sk_prot->compat_setsockopt(sk, level, optname,
						      optval, optlen);
	return sk->sk_prot->setsockopt(sk, level, optname, optval, optlen);
}