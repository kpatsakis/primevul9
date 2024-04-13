static inline struct sk_buff *alloc_skb_head(gfp_t priority)
{
	return __alloc_skb_head(priority, -1);
}