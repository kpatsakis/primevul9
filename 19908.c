static int ovs_nla_put_vlan(struct sk_buff *skb, const struct vlan_head *vh,
			    bool is_mask)
{
	__be16 eth_type = !is_mask ? vh->tpid : htons(0xffff);

	if (nla_put_be16(skb, OVS_KEY_ATTR_ETHERTYPE, eth_type) ||
	    nla_put_be16(skb, OVS_KEY_ATTR_VLAN, vh->tci))
		return -EMSGSIZE;
	return 0;
}