static inline u16 skb_get_rx_queue(const struct sk_buff *skb)
{
	return skb->queue_mapping - 1;
}