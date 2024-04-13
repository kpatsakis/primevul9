static inline void nf_conntrack_put_reasm(struct sk_buff *skb)
{
	if (skb)
		kfree_skb(skb);
}