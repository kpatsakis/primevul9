static inline bool skb_pfmemalloc(const struct sk_buff *skb)
{
	return unlikely(skb->pfmemalloc);
}