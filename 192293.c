static inline bool skb_has_frag_list(const struct sk_buff *skb)
{
	return skb_shinfo(skb)->frag_list != NULL;
}