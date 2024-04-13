static inline void skb_tx_timestamp(struct sk_buff *skb)
{
	skb_clone_tx_timestamp(skb);
	sw_tx_timestamp(skb);
}