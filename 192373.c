static inline bool skb_head_is_locked(const struct sk_buff *skb)
{
	return !skb->head_frag || skb_cloned(skb);
}