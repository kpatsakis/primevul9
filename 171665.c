void sk_set_memalloc(struct sock *sk)
{
	sock_set_flag(sk, SOCK_MEMALLOC);
	sk->sk_allocation |= __GFP_MEMALLOC;
	static_key_slow_inc(&memalloc_socks);
}