static inline void skb_dst_set(struct sk_buff *skb, struct dst_entry *dst)
{
	skb->_skb_refdst = (unsigned long)dst;
}