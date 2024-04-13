static inline bool skb_is_nonlinear(const struct sk_buff *skb)
{
	return skb->data_len;
}