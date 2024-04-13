static inline unsigned int skb_headlen(const struct sk_buff *skb)
{
	return skb->len - skb->data_len;
}