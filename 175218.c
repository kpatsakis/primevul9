static int ovs_flow_cmd_new_or_set(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr **a = info->attrs;
	struct ovs_header *ovs_header = info->userhdr;
	struct sw_flow_key key, masked_key;
	struct sw_flow *flow = NULL;
	struct sw_flow_mask mask;
	struct sk_buff *reply;
	struct datapath *dp;
	struct sw_flow_actions *acts = NULL;
	struct sw_flow_match match;
	bool exact_5tuple;
	int error;

	/* Extract key. */
	error = -EINVAL;
	if (!a[OVS_FLOW_ATTR_KEY])
		goto error;

	ovs_match_init(&match, &key, &mask);
	error = ovs_nla_get_match(&match, &exact_5tuple,
				  a[OVS_FLOW_ATTR_KEY], a[OVS_FLOW_ATTR_MASK]);
	if (error)
		goto error;

	/* Validate actions. */
	if (a[OVS_FLOW_ATTR_ACTIONS]) {
		acts = ovs_nla_alloc_flow_actions(nla_len(a[OVS_FLOW_ATTR_ACTIONS]));
		error = PTR_ERR(acts);
		if (IS_ERR(acts))
			goto error;

		ovs_flow_mask_key(&masked_key, &key, &mask);
		error = ovs_nla_copy_actions(a[OVS_FLOW_ATTR_ACTIONS],
					     &masked_key, 0, &acts);
		if (error) {
			OVS_NLERR("Flow actions may not be safe on all matching packets.\n");
			goto err_kfree;
		}
	} else if (info->genlhdr->cmd == OVS_FLOW_CMD_NEW) {
		error = -EINVAL;
		goto error;
	}

	ovs_lock();
	dp = get_dp(sock_net(skb->sk), ovs_header->dp_ifindex);
	error = -ENODEV;
	if (!dp)
		goto err_unlock_ovs;

	/* Check if this is a duplicate flow */
	flow = ovs_flow_tbl_lookup(&dp->table, &key);
	if (!flow) {
		/* Bail out if we're not allowed to create a new flow. */
		error = -ENOENT;
		if (info->genlhdr->cmd == OVS_FLOW_CMD_SET)
			goto err_unlock_ovs;

		/* Allocate flow. */
		flow = ovs_flow_alloc(!exact_5tuple);
		if (IS_ERR(flow)) {
			error = PTR_ERR(flow);
			goto err_unlock_ovs;
		}

		flow->key = masked_key;
		flow->unmasked_key = key;
		rcu_assign_pointer(flow->sf_acts, acts);

		/* Put flow in bucket. */
		error = ovs_flow_tbl_insert(&dp->table, flow, &mask);
		if (error) {
			acts = NULL;
			goto err_flow_free;
		}

		reply = ovs_flow_cmd_build_info(flow, dp, info, OVS_FLOW_CMD_NEW);
	} else {
		/* We found a matching flow. */
		struct sw_flow_actions *old_acts;

		/* Bail out if we're not allowed to modify an existing flow.
		 * We accept NLM_F_CREATE in place of the intended NLM_F_EXCL
		 * because Generic Netlink treats the latter as a dump
		 * request.  We also accept NLM_F_EXCL in case that bug ever
		 * gets fixed.
		 */
		error = -EEXIST;
		if (info->genlhdr->cmd == OVS_FLOW_CMD_NEW &&
		    info->nlhdr->nlmsg_flags & (NLM_F_CREATE | NLM_F_EXCL))
			goto err_unlock_ovs;

		/* The unmasked key has to be the same for flow updates. */
		if (!ovs_flow_cmp_unmasked_key(flow, &match))
			goto err_unlock_ovs;

		/* Update actions. */
		old_acts = ovsl_dereference(flow->sf_acts);
		rcu_assign_pointer(flow->sf_acts, acts);
		ovs_nla_free_flow_actions(old_acts);

		reply = ovs_flow_cmd_build_info(flow, dp, info, OVS_FLOW_CMD_NEW);

		/* Clear stats. */
		if (a[OVS_FLOW_ATTR_CLEAR])
			ovs_flow_stats_clear(flow);
	}
	ovs_unlock();

	if (!IS_ERR(reply))
		ovs_notify(&dp_flow_genl_family, reply, info);
	else
		genl_set_err(&dp_flow_genl_family, sock_net(skb->sk), 0,
			     0, PTR_ERR(reply));
	return 0;

err_flow_free:
	ovs_flow_free(flow, false);
err_unlock_ovs:
	ovs_unlock();
err_kfree:
	kfree(acts);
error:
	return error;
}