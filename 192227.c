static inline void skb_reserve(struct sk_buff *skb, int len)
{
	skb->data += len;
	skb->tail += len;
}