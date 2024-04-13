static int validate_and_copy_clone(struct net *net,
				   const struct nlattr *attr,
				   const struct sw_flow_key *key,
				   struct sw_flow_actions **sfa,
				   __be16 eth_type, __be16 vlan_tci,
				   u32 mpls_label_count, bool log, bool last)
{
	int start, err;
	u32 exec;

	if (nla_len(attr) && nla_len(attr) < NLA_HDRLEN)
		return -EINVAL;

	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_CLONE, log);
	if (start < 0)
		return start;

	exec = last || !actions_may_change_flow(attr);

	err = ovs_nla_add_action(sfa, OVS_CLONE_ATTR_EXEC, &exec,
				 sizeof(exec), log);
	if (err)
		return err;

	err = __ovs_nla_copy_actions(net, attr, key, sfa,
				     eth_type, vlan_tci, mpls_label_count, log);
	if (err)
		return err;

	add_nested_action_end(*sfa, start);

	return 0;
}