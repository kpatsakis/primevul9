static inline int skb_cow_head(struct sk_buff *skb, unsigned int headroom)
{
	return __skb_cow(skb, headroom, skb_header_cloned(skb));
}