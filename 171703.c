void sk_clear_memalloc(struct sock *sk)
{
	sock_reset_flag(sk, SOCK_MEMALLOC);
	sk->sk_allocation &= ~__GFP_MEMALLOC;
	static_key_slow_dec(&memalloc_socks);

	/*
	 * SOCK_MEMALLOC is allowed to ignore rmem limits to ensure forward
	 * progress of swapping. SOCK_MEMALLOC may be cleared while
	 * it has rmem allocations due to the last swapfile being deactivated
	 * but there is a risk that the socket is unusable due to exceeding
	 * the rmem limits. Reclaim the reserves and obey rmem limits again.
	 */
	sk_mem_reclaim(sk);
}