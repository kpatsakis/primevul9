static inline struct sk_buff *skb_get(struct sk_buff *skb)
{
	atomic_inc(&skb->users);
	return skb;
}