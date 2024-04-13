static inline int skb_unclone(struct sk_buff *skb, gfp_t pri)
{
	might_sleep_if(pri & __GFP_WAIT);

	if (skb_cloned(skb))
		return pskb_expand_head(skb, 0, 0, pri);

	return 0;
}