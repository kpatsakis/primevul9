static inline struct rtable *skb_rtable(const struct sk_buff *skb)
{
	return (struct rtable *)skb_dst(skb);
}