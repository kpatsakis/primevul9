void consume_skb(struct sk_buff *skb)
{
	if (unlikely(!skb))
		return;
	if (likely(atomic_read(&skb->users) == 1))
		smp_rmb();
	else if (likely(!atomic_dec_and_test(&skb->users)))
		return;
	trace_consume_skb(skb);
	__kfree_skb(skb);
}