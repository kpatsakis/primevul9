static inline int skb_availroom(const struct sk_buff *skb)
{
	if (skb_is_nonlinear(skb))
		return 0;

	return skb->end - skb->tail - skb->reserved_tailroom;
}