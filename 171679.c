int compat_sock_common_getsockopt(struct socket *sock, int level, int optname,
				  char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;

	if (sk->sk_prot->compat_getsockopt != NULL)
		return sk->sk_prot->compat_getsockopt(sk, level, optname,
						      optval, optlen);
	return sk->sk_prot->getsockopt(sk, level, optname, optval, optlen);
}