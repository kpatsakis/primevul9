static inline bool skb_is_gso(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->gso_size;
}