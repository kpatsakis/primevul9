int ovs_nla_put_identifier(const struct sw_flow *flow, struct sk_buff *skb)
{
	if (ovs_identifier_is_ufid(&flow->id))
		return nla_put(skb, OVS_FLOW_ATTR_UFID, flow->id.ufid_len,
			       flow->id.ufid);

	return ovs_nla_put_key(flow->id.unmasked_key, flow->id.unmasked_key,
			       OVS_FLOW_ATTR_KEY, false, skb);
}