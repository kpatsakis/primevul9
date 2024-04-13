static inline void skb_set_queue_mapping(struct sk_buff *skb, u16 queue_mapping)
{
	skb->queue_mapping = queue_mapping;
}