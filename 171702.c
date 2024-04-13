struct sk_buff *sock_wmalloc(struct sock *sk, unsigned long size, int force,
			     gfp_t priority)
{
	if (force || refcount_read(&sk->sk_wmem_alloc) < sk->sk_sndbuf) {
		struct sk_buff *skb = alloc_skb(size, priority);
		if (skb) {
			skb_set_owner_w(skb, sk);
			return skb;
		}
	}
	return NULL;
}