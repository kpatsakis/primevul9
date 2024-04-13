bool sk_mc_loop(struct sock *sk)
{
	if (dev_recursion_level())
		return false;
	if (!sk)
		return true;
	switch (sk->sk_family) {
	case AF_INET:
		return inet_sk(sk)->mc_loop;
#if IS_ENABLED(CONFIG_IPV6)
	case AF_INET6:
		return inet6_sk(sk)->mc_loop;
#endif
	}
	WARN_ON(1);
	return true;
}