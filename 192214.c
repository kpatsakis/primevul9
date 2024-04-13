static inline bool skb_queue_is_last(const struct sk_buff_head *list,
				     const struct sk_buff *skb)
{
	return skb->next == (struct sk_buff *)list;
}