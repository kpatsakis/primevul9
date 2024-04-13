static inline int skb_cow(struct sk_buff *skb, unsigned int headroom)
{
	return __skb_cow(skb, headroom, skb_cloned(skb));
}