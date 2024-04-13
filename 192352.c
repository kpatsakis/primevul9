static inline void skb_record_rx_queue(struct sk_buff *skb, u16 rx_queue)
{
	skb->queue_mapping = rx_queue + 1;
}