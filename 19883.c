int ovs_nla_copy_actions(struct net *net, const struct nlattr *attr,
			 const struct sw_flow_key *key,
			 struct sw_flow_actions **sfa, bool log)
{
	int err;
	u32 mpls_label_count = 0;

	*sfa = nla_alloc_flow_actions(min(nla_len(attr), MAX_ACTIONS_BUFSIZE));
	if (IS_ERR(*sfa))
		return PTR_ERR(*sfa);

	if (eth_p_mpls(key->eth.type))
		mpls_label_count = hweight_long(key->mpls.num_labels_mask);

	(*sfa)->orig_len = nla_len(attr);
	err = __ovs_nla_copy_actions(net, attr, key, sfa, key->eth.type,
				     key->eth.vlan.tci, mpls_label_count, log);
	if (err)
		ovs_nla_free_flow_actions(*sfa);

	return err;
}