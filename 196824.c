u32 skb_get_poff(const struct sk_buff *skb)
{
	struct flow_keys keys;

	if (!skb_flow_dissect_flow_keys(skb, &keys, 0))
		return 0;

	return __skb_get_poff(skb, skb->data, &keys, skb_headlen(skb));
}