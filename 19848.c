int ovs_nla_get_flow_metadata(struct net *net,
			      const struct nlattr *a[OVS_KEY_ATTR_MAX + 1],
			      u64 attrs, struct sw_flow_key *key, bool log)
{
	struct sw_flow_match match;

	memset(&match, 0, sizeof(match));
	match.key = key;

	key->ct_state = 0;
	key->ct_zone = 0;
	key->ct_orig_proto = 0;
	memset(&key->ct, 0, sizeof(key->ct));
	memset(&key->ipv4.ct_orig, 0, sizeof(key->ipv4.ct_orig));
	memset(&key->ipv6.ct_orig, 0, sizeof(key->ipv6.ct_orig));

	key->phy.in_port = DP_MAX_PORTS;

	return metadata_from_nlattrs(net, &match, &attrs, a, false, log);
}