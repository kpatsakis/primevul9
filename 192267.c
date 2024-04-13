static inline bool skb_queue_is_first(const struct sk_buff_head *list,
				      const struct sk_buff *skb)
{
	return skb->prev == (struct sk_buff *)list;
}