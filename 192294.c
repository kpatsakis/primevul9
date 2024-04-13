static inline int skb_cloned(const struct sk_buff *skb)
{
	return skb->cloned &&
	       (atomic_read(&skb_shinfo(skb)->dataref) & SKB_DATAREF_MASK) != 1;
}