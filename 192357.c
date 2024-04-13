static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->head + skb->end;
}