static struct sk_buff *netlink_trim(struct sk_buff *skb, gfp_t allocation)
{
	int delta;

	WARN_ON(skb->sk != NULL);
	delta = skb->end - skb->tail;
	if (is_vmalloc_addr(skb->head) || delta * 2 < skb->truesize)
		return skb;

	if (skb_shared(skb)) {
		struct sk_buff *nskb = skb_clone(skb, allocation);
		if (!nskb)
			return skb;
		consume_skb(skb);
		skb = nskb;
	}

	pskb_expand_head(skb, 0, -delta,
			 (allocation & ~__GFP_DIRECT_RECLAIM) |
			 __GFP_NOWARN | __GFP_NORETRY);
	return skb;
}