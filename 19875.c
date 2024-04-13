int ovs_nla_put_mask(const struct sw_flow *flow, struct sk_buff *skb)
{
	return ovs_nla_put_key(&flow->key, &flow->mask->key,
				OVS_FLOW_ATTR_MASK, true, skb);
}