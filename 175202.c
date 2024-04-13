static int ovs_flow_cmd_del(struct sk_buff *skb, struct genl_info *info)
{
	struct nlattr **a = info->attrs;
	struct ovs_header *ovs_header = info->userhdr;
	struct sw_flow_key key;
	struct sk_buff *reply;
	struct sw_flow *flow;
	struct datapath *dp;
	struct sw_flow_match match;
	int err;

	ovs_lock();
	dp = get_dp(sock_net(skb->sk), ovs_header->dp_ifindex);
	if (!dp) {
		err = -ENODEV;
		goto unlock;
	}

	if (!a[OVS_FLOW_ATTR_KEY]) {
		err = ovs_flow_tbl_flush(&dp->table);
		goto unlock;
	}

	ovs_match_init(&match, &key, NULL);
	err = ovs_nla_get_match(&match, NULL, a[OVS_FLOW_ATTR_KEY], NULL);
	if (err)
		goto unlock;

	flow = ovs_flow_tbl_lookup(&dp->table, &key);
	if (!flow || !ovs_flow_cmp_unmasked_key(flow, &match)) {
		err = -ENOENT;
		goto unlock;
	}

	reply = ovs_flow_cmd_alloc_info(flow, info);
	if (!reply) {
		err = -ENOMEM;
		goto unlock;
	}

	ovs_flow_tbl_remove(&dp->table, flow);

	err = ovs_flow_cmd_fill_info(flow, dp, reply, info->snd_portid,
				     info->snd_seq, 0, OVS_FLOW_CMD_DEL);
	BUG_ON(err < 0);

	ovs_flow_free(flow, true);
	ovs_unlock();

	ovs_notify(&dp_flow_genl_family, reply, info);
	return 0;
unlock:
	ovs_unlock();
	return err;
}