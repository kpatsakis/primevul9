static inline int skb_linearize(struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? __skb_linearize(skb) : 0;
}