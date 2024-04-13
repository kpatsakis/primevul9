static inline int skb_alloc_rx_flag(const struct sk_buff *skb)
{
	if (skb_pfmemalloc(skb))
		return SKB_ALLOC_RX;
	return 0;
}