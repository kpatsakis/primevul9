static inline int skb_csum_unnecessary(const struct sk_buff *skb)
{
	return skb->ip_summed & CHECKSUM_UNNECESSARY;
}