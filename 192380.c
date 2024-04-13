static inline struct skb_shared_hwtstamps *skb_hwtstamps(struct sk_buff *skb)
{
	return &skb_shinfo(skb)->hwtstamps;
}