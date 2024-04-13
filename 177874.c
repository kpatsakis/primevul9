_l2_alloc_skb(unsigned int len, gfp_t gfp_mask)
{
	struct sk_buff  *skb;

	skb = alloc_skb(len + L2_HEADER_LEN, gfp_mask);
	if (likely(skb))
		skb_reserve(skb, L2_HEADER_LEN);
	return skb;
}