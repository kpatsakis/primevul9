static int encode_vlan_from_nlattrs(struct sw_flow_match *match,
				    const struct nlattr *a[],
				    bool is_mask, bool inner)
{
	__be16 tci = 0;
	__be16 tpid = 0;

	if (a[OVS_KEY_ATTR_VLAN])
		tci = nla_get_be16(a[OVS_KEY_ATTR_VLAN]);

	if (a[OVS_KEY_ATTR_ETHERTYPE])
		tpid = nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE]);

	if (likely(!inner)) {
		SW_FLOW_KEY_PUT(match, eth.vlan.tpid, tpid, is_mask);
		SW_FLOW_KEY_PUT(match, eth.vlan.tci, tci, is_mask);
	} else {
		SW_FLOW_KEY_PUT(match, eth.cvlan.tpid, tpid, is_mask);
		SW_FLOW_KEY_PUT(match, eth.cvlan.tci, tci, is_mask);
	}
	return 0;
}