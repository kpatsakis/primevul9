static inline __u32 skb_get_rxhash(struct sk_buff *skb)
{
	if (!skb->l4_rxhash)
		__skb_get_rxhash(skb);

	return skb->rxhash;
}