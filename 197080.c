u32 bond_xmit_hash(struct bonding *bond, struct sk_buff *skb)
{
	struct flow_keys flow;
	u32 hash;

	if (bond->params.xmit_policy == BOND_XMIT_POLICY_ENCAP34 &&
	    skb->l4_hash)
		return skb->hash;

	if (bond->params.xmit_policy == BOND_XMIT_POLICY_VLAN_SRCMAC)
		return bond_vlan_srcmac_hash(skb);

	if (bond->params.xmit_policy == BOND_XMIT_POLICY_LAYER2 ||
	    !bond_flow_dissect(bond, skb, &flow))
		return bond_eth_hash(skb);

	if (bond->params.xmit_policy == BOND_XMIT_POLICY_LAYER23 ||
	    bond->params.xmit_policy == BOND_XMIT_POLICY_ENCAP23) {
		hash = bond_eth_hash(skb);
	} else {
		if (flow.icmp.id)
			memcpy(&hash, &flow.icmp, sizeof(hash));
		else
			memcpy(&hash, &flow.ports.ports, sizeof(hash));
	}

	return bond_ip_hash(hash, &flow);
}