static int skb_prepare_for_shift(struct sk_buff *skb)
{
	return skb_cloned(skb) && pskb_expand_head(skb, 0, 0, GFP_ATOMIC);
}