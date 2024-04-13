static inline void skb_copy_queue_mapping(struct sk_buff *to, const struct sk_buff *from)
{
	to->queue_mapping = from->queue_mapping;
}