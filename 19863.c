int ovs_nla_put_masked_key(const struct sw_flow *flow, struct sk_buff *skb)
{
	return ovs_nla_put_key(&flow->key, &flow->key,
				OVS_FLOW_ATTR_KEY, false, skb);
}