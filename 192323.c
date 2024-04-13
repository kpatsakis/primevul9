static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data - skb->head;
}