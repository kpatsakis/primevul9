static inline struct sec_path *skb_sec_path(struct sk_buff *skb)
{
	return skb->sp;
}