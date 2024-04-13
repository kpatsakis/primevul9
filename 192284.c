static inline void skb_dst_set_noref(struct sk_buff *skb, struct dst_entry *dst)
{
	__skb_dst_set_noref(skb, dst, false);
}