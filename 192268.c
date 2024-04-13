static inline void skb_dst_set_noref_force(struct sk_buff *skb,
					   struct dst_entry *dst)
{
	__skb_dst_set_noref(skb, dst, true);
}