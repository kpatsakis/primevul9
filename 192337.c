static inline bool skb_rx_queue_recorded(const struct sk_buff *skb)
{
	return skb->queue_mapping != 0;
}