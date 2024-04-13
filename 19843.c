static int validate_and_copy_dec_ttl(struct net *net,
				     const struct nlattr *attr,
				     const struct sw_flow_key *key,
				     struct sw_flow_actions **sfa,
				     __be16 eth_type, __be16 vlan_tci,
				     u32 mpls_label_count, bool log)
{
	const struct nlattr *attrs[OVS_DEC_TTL_ATTR_MAX + 1];
	int start, action_start, err, rem;
	const struct nlattr *a, *actions;

	memset(attrs, 0, sizeof(attrs));
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		/* Ignore unknown attributes to be future proof. */
		if (type > OVS_DEC_TTL_ATTR_MAX)
			continue;

		if (!type || attrs[type]) {
			OVS_NLERR(log, "Duplicate or invalid key (type %d).",
				  type);
			return -EINVAL;
		}

		attrs[type] = a;
	}

	if (rem) {
		OVS_NLERR(log, "Message has %d unknown bytes.", rem);
		return -EINVAL;
	}

	actions = attrs[OVS_DEC_TTL_ATTR_ACTION];
	if (!actions || (nla_len(actions) && nla_len(actions) < NLA_HDRLEN)) {
		OVS_NLERR(log, "Missing valid actions attribute.");
		return -EINVAL;
	}

	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_DEC_TTL, log);
	if (start < 0)
		return start;

	action_start = add_nested_action_start(sfa, OVS_DEC_TTL_ATTR_ACTION, log);
	if (action_start < 0)
		return action_start;

	err = __ovs_nla_copy_actions(net, actions, key, sfa, eth_type,
				     vlan_tci, mpls_label_count, log);
	if (err)
		return err;

	add_nested_action_end(*sfa, action_start);
	add_nested_action_end(*sfa, start);
	return 0;
}