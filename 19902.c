static int parse_eth_type_from_nlattrs(struct sw_flow_match *match,
				       u64 *attrs, const struct nlattr **a,
				       bool is_mask, bool log)
{
	__be16 eth_type;

	eth_type = nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE]);
	if (is_mask) {
		/* Always exact match EtherType. */
		eth_type = htons(0xffff);
	} else if (!eth_proto_is_802_3(eth_type)) {
		OVS_NLERR(log, "EtherType %x is less than min %x",
				ntohs(eth_type), ETH_P_802_3_MIN);
		return -EINVAL;
	}

	SW_FLOW_KEY_PUT(match, eth.type, eth_type, is_mask);
	*attrs &= ~(1 << OVS_KEY_ATTR_ETHERTYPE);
	return 0;
}