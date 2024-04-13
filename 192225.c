static inline u16 skb_get_queue_mapping(const struct sk_buff *skb)
{
	return skb->queue_mapping;
}