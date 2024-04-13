void skb_orphan_partial(struct sk_buff *skb)
{
	if (skb_is_tcp_pure_ack(skb))
		return;

	if (skb->destructor == sock_wfree
#ifdef CONFIG_INET
	    || skb->destructor == tcp_wfree
#endif
		) {
		struct sock *sk = skb->sk;

		if (refcount_inc_not_zero(&sk->sk_refcnt)) {
			WARN_ON(refcount_sub_and_test(skb->truesize, &sk->sk_wmem_alloc));
			skb->destructor = sock_efree;
		}
	} else {
		skb_orphan(skb);
	}
}