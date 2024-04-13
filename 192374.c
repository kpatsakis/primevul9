static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}